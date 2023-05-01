# Perfect Info

Pretty much always, we assume the battle is perfect info. This means anything that is normally only known to one player (besides the move they just selected) is known by both players. Technically, the parens in the last sentence means the game is imperfect info, when considered as a general game. But it's perfect info when considered as a simultaneous move game.

The reason for this assumption is simplicity. Lookahead in perfect Info SM games is much simpler and less expensive than imperfect in both runtime and learning contexts. Search algorithms mirror those of traditional perfect info games, the key difference being that it is the Nash equilibrium payoffs at a node that are propagated down to the root when we a recursively defining the 'value' of a game, in the minimax sense. 
>	In the 2-player constant sum case (we need both those conditions) the payoffs of every NE are 	identical for both players, and are also interchangeable in the following sense: Suppose (x,y) are NE strategies for row and column player (Nash equilbrium is only defined on pairs) and so are (x', y'). Then (x, y') and (x',y) are also NE.

## Imperfect Info

Provable search in imperfect info games is in the form of 'Counterfactual Regret Minimization'. It is much more challenging. "Player of Games" used a new kind of CRM called 'safe subgame solving' which is a close analog of AlphaZero but in the imperfect info context.

There are no plans for Surskit to support CRM style search and I'm not going to be pursuing this for a while. These no strong evaluation functions for pokemon currently and it doesn't seem easy to make one. And pure monte carlo is terrible so I think RL methods are the best approach.

Alpha Zero is much better understood than Player of Games and there are much more resources for it. Also naive applying PoG to pokemon is intractable, since the 'belief' space is so large.

### Bridging the Gap

Pokemon has exploitable properties about its information type. Namely it is 'fixed' and revealed over the course of the game in ways that are usually unambiguous. Essentially players are just discovering their opponents sets, and the only observations that are ambiguous are those relating to stats. Stats are quasi continuous (integer values but behave more like real numbers and should probably be treated as such) and the speed stat has a much higher perceived resolution because of how speed relates to turn order. Stats (and importantly just the binary relations '<', '=' between opposing speed stats rather than the actual speed, most of the time)  can be calculated over the course of the game with high certainty.

Thus the game can eventually leak all information and become perfect info. And in the transition between the start of the battle to perfect info, you can apply restricted perfect-info lookahead in some cases. This kind of approach would be tailed specifically to pokemon and is not in the literature afaik. Still, It's clear that some kind of lookahead is necessary even in the imperfect info contexts. My advice is to think about how people do it.

## Perfect info is good enough

Considering the above, games frequently settle into a perfect information state. Perfect info search is very useful most games in fact. If stats are estimated to high certainty then search using high likelihood stand-in values for the opposing stats would produce true results in most cases.

A strong perfect info search would have lots of applications, particularly in analysis-engines and tactics trainers.

## MCTS

"Monte carlo" is not necessarily pure. It can use a NN evaluation for value and policy estimation at each newly expanded matrix node. It does not necessarily need a policy estimate nor does it have to be an NN; A traditional value heuristic is fine too. So basically 'mcts' in popular usage refers to the scheme of growing the tree incrementally, one node at a time. There is also typically a bandit algorithm that is being applied at each matrix node that is being visited as we traverse up the explored sub-tree.

### Rollout vs Playout
[This](https://towardsdatascience.com/monte-carlo-tree-search-in-reinforcement-learning-b97d3e743d0f) article uses the term 'rollout' to refer to the process of selecting random moves until terminal at each node. The wiki article uses 'playout' for this. In Surskit I use 'playout' for traversal to leaf node of explored tree and 'rollout' for the random moves until terminal procedure.

The payoff/value at the terminal node in the alternating move case is an 'unbiased payoff observation' in the sense of [here](https://arxiv.org/pdf/1509.00149.pdf). The same is true in the SM case but with one small caveat. The previous paper shows that the *average* of the values has the UPB property, and thus *provably converges*. The authors show that the raw payoffs do not lead to NE convergence on a contrived game but speculate the probably do for most games. They use the terms SM-MCTS(-A) to distinguish these types of search. The unaveraged rewards lead to much faster convergence in practice.

## Models
In Surskit, the thing that provides a value (or policy) about a battle/state, when a matrix node is being expanded is called a *model*. It might also provide other information such as likelihood estimates, but that is unexplored and mostly relates to imperfect info. Besides random rollouts, there are a few candidate options for model implementation.

* Small NN
This is the simplest option imo. There is no conventional wisdom regarding architecture or hyperparameters. I think a small stack of transformer encoders or MLPs should be tried. NNEU is worth considering.

* Deep NN
	This presents challenges because of latency between GPU and CPU. Inference has to be done in batches. LeelaChess works this way and it their discord server is a good resource. Batched inference is the primary speed bottleneck, I believe.

* Handcraft Eval
	Pasy is working on this. It seems pretty tricky but I suspect all you need is a good backbone/battle representation and then fine tuning via evolutionary algorithms or something should be promising.

## Bandits

SM-MCTS(-A) uses two adversarial bandit algorithms. There is a folk theorem that the empirical strategies of two opposing adversarial-bandit algos in a matrix game will converge to NE. Unfortunately this is *very slow*

 There are other good candidates for search algorithms in general but the best bet for MCTS style is certainly [MatrixUCB](https://arxiv.org/abs/2006.05145). This paper notes that adversarial bandits perform poorly in comparison. When testing 'Stunfisk' (my first search impl in PS) I found that its main weakness is its inability to rule out obviously bad actions. The reason for this is clear since an adversarial bandit algorithm is one that basically works when there are no assumptions at all on the rewards, so it can't exploit the matrix structure.

Surskit designed to work with any joint-action selection process, so it supports Exp3/p and MatrixUCB.  In contrast MatrixUCB is a stochastic bandit algorithm because it makes the selection for both row and column player, so it only has to deal with the stochasticity inherent from pokemons RNG.

MatrixUCB is also a direct generalization of UCB (The latter assumes the matrix has the shape (1xn) or (nx1)), so it is easy to implement MatrixPUCB. This is an analogue of PUCT/PUCB like in AlphaZero. It uses the policy estimation of the network in the tree search to prefer network endorsed actions and thus practically reduce the branching factor. We simply weight the exploration term of the joint action (a_i, a_j) by its probability of being played, which is the product of the row and column players individual policies.

## Testing

Surskit is designed to allow easy comparison of these MCTS algorithms and tuning of their search parameters.

## Multithreading

There is probably a lot of room for improvement in Surskit's multithreading. Currently each thread has two wait twice at the root node to lock the node's statistics during selection and backprop/update. The contention here could probably be reduced somehow or even the threads could be moved up the tree, where there would be multiple 'root nodes' and thus lower contention at each one. Pre probably has more/better ideas.

# Tree Structure

Surskit uses two kinds of nodes to accommodate stochastic games. The matrix node is a decision point for both players, and a chance node is a decision point for the 'chance' player. A matrix node has edges (each corresponding to a joint action) pointing to chance nodes. Chance nodes have edges (corresponding to the possible rng 'outcomes' that point to MatrixNodes. It is matrix nodes that store decision making statistics and require expanding by MCTS. Chance nodes basically just hold a linked list of edges (although they can also store stats and that might be convenient).

Each of these edges at the chance node has an identifying `Transition` object. This object stores the `Observation` which is how different outcomes are identified: by `==` of their observations. It also stores the `Probabilty` of that outcome. We don't need to know the probability for any of the SM-MCTS algorithms to work, since the chance player samples naturally, but it will be necessary info for more 'Stockfish Like' searches to work, in the same way Expectiminimax needs those probabilities to return the weighted average.

A stochastic two player SM game could also be thought of as an (m, n, k) matrix, where the chance player k actions. Here there are no chance nodes. The current scheme is simpler for many reasons imo.

The current tree structure is Surskit is general enough to work with non MCTS algorithms and even imperfect info search algorithms.

## Pruning Edges at Chance Nodes

This is an extremely important task if a search is going to be effective. There are simply way to many possible outcomes if we identify edges by prng calls, almost soley because of damage rolls across gens.

## Other Applications

# Performance Optimization

## Solving Matrix Games

## Unclear

# Itinerary of Future Work

