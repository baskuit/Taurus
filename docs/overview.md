
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

# MCTS

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


## Multithreading

There is probably a lot of room for improvement in Surskit's multithreading. Currently each thread has two wait twice at the root node (and every matrix node) to lock the node's statistics during selection and backprop/update. The contention here could probably be reduced somehow or even the threads could be moved up the tree, where there would be multiple 'root nodes' and thus lower contention at each one. Pre probably has more/better ideas regarding this.

# Tree Structure

Surskit uses two kinds of nodes to accommodate stochastic games. The matrix node is a decision point for both players, and a chance node is a decision point for the 'chance' player. A matrix node has edges (each corresponding to a joint action) pointing to chance nodes. Chance nodes have edges (corresponding to the possible rng 'outcomes' that point to MatrixNodes. It is matrix nodes that store decision making statistics and require expanding by MCTS. Chance nodes basically just hold a linked list of edges (although they can also store stats and that might be convenient).

Each of these edges at the chance node has an identifying `Transition` object. This object stores the `Observation` which is how different outcomes are identified: by `==` of their observations. It also stores the `Probabilty` of that outcome. We don't need to know the probability for any of the SM-MCTS algorithms to work, since the chance player samples naturally, but it will be necessary info for more 'Stockfish Like' searches to work, in the same way Expectiminimax needs those probabilities to return the weighted average.

A stochastic two player SM game could also be thought of as an (m, n, k) matrix, where the chance player k actions. Here there are no chance nodes. The current scheme is simpler for many reasons imo.

The current tree structure is Surskit is general enough to work with non MCTS algorithms and even imperfect info search algorithms.

## Edges at Chance Nodes aka Transitions

This is an extremely important consideration if search is going to be effective. There are simply way too many possible outcomes if we identify edges by prng calls, almost soley because of damage rolls across gens. We aren't so much 'pruning' the edges at chance nodes than 'identifying' them together. Either way the desired result is a reduction in the branching factor.
There are two main categories of identifying chance node edges: lossless and lossy.

### Lossless
This means that we are not identifying edges that lead to distinct states together. There are several schemes of identification. The ones resulting in less branches require more calculation, not only because of calculation of the probability of that branch.

* Log
	Here we just the showdown log to differentiate between branches, nothing else. 
* State
This is the most extreme identification that is lossless. We simply compare the state at the end for equality of bits. Practically speaking I'm not sure if this is possible without hashing, and the possibility of hash collisions means we no longer have the 'lossless' property guaranteed.

### Lossy

We could simply identify two branches together if the the total damage done by the respective players is close enough. This would reduce the branching factor due to damage rolls immensely, and that reduction could be tuned.

Another approach would be to simply change the **simulator**, so that the full number (but maybe keep range intact) of damage rolls are no longer possible. This is what I did in my original Stunfisk search. There were only 3 rolls: low, med, and high. They had probabilities of 1/4, 1/2, and 1/4 resp.

These two methods cause different kinds of inconsistencies: 

The former means that when a deeper node is visited, the underlying battle that is guiding the tree traversal could be different than the battle that was previously 'visiting' that node. If they are different enough, we could have an 'action space desync' where an action that is recorded at the matrix node as being a legal option is not legal at the current battle. There are many kinds of pathologies that could happen because of this 'drift' between the battles that accompany any matrix node. These pathologies would have to be handled in the tree to prevent the battle from turning into garbage. If you play an invalid action currently you risk the battle never returning a terminal state, so the playout would go on forever and the search would stall.

The latter means that the game being searched is now only an approximation of the actual game of pokemon. However this approximation is also tunable and there is the added benefit that these inconsistencies don't have to be handled within the tree and don't lead to runtime errors. The only risk is that the results of the search don't reflect the reality of the game you intend to search. I speculate that the battle states where this happens in a significant way are rare.

## Probability Calculation

In your doc on `-Dchance`, you've only been considering lossless methods. That's fine, and I will leave all those considerations to your doc :) 

## Other Applications

It should be simple to do search-free methods like DeepNash using surskit + engine too. Eventually I want to write some general "Trajectory" (RL term) data classes for alpha zero and these could also be used to generate data for DeepNash and other policy gradient methods just fine.

# Performance Optimization

Speed is paramount whether we are doing RL or even just search with a preexisting evaluator. Progress in computer chess has basically been due to optimizations and hardware improvements.

## Solving Matrix Games

My discussions with Dr. Savani have indicated that full precision arithmetic with Extreme Equilibrium Enumeration of Lemke Howsen should be the fastest way to solve the 'larger' matrices in our use case. This is because the floating point Gambit solver was shitting the bed and doing tens of thousands of pivots for some matrices. This was not a bug with my implementation, but rather a limitation of the software.

My MatrixUCB implementations will memoize the NE strategies at each node and only resolve after checking the exploitability of the old strategies on the newly perturbed matrix is above some threshold. The strategies are initialized to uniform which is in fact at NE on a fresh UCB bi-matrix, since the later will also be uniform.
Since most nodes in MCTS are leaf nodes and are thus only visited a handful of times, this means that we generally don't have to actually solve matrices any where but the lowest depths of the tree. Still, depending on the exploitability threshold a 'mature' UCB matrix will have to be solved every few visits. Thus fast solving for 9x9 matrices is very important. In fact my MatrixUCB search is orders of magnitude slower than my Exp3p search for that fact alone; all other aspects between these two searches are nearly identical.

## Unclear

Besides matrix solving and my multithreading, I'm not sure what needs attention. I think the `ChanceNode::access` function that takes a `Transition` object and either points to the matrix node child that corresponds to it or creates a new one could be improved, probably.

# AlphaBeta

## In the SM context

AlphaBeta is an optimization of the minimax algorithm for alternating move games that allows for the calculation of the root node *value* (It's game theoretic value to be precise) without visiting and estimating/inferencing every single leaf node. I'm not going to explain how the algorithm works; there are much better resources online for doing that.

The minimax algorithm has a direct parallel in the perfect info SM case. The only distinction is that we the value of a node is the NE payoff (for the row player) of the matrix of (row payoff) values for the subsequent child nodes. Again, the game being 2 player and constant sum means that *the NE payoff* is well defined, since the expected payoffs for any NE are the same. Stochasticity does not change much. In that case, joint actions point to a chance node instead of another matrix node, and the payoff of the chance node is the expected payoff of all the matrix nodes that the chance node shares an edge with (weighted by the probability of that edge/transition).

There are two papers that can be easily found re SM alphabeta. The first: [Alpha Beta pruning for Games with Simultaneous Moves](https://ojs.aaai.org/index.php/AAAI/article/view/8148) (Saffadine) was published in 2012 and a later paper: [Using Double-Oracle Method and Serialized Alpha-Beta Search  for Pruning in Simultaneous Move Games](https://www.ijcai.org/Proceedings/13/Papers/018.pdf)
(Bosansky) claims to be an improvement. In the best case, only 7% of the terminal nodes need to be visited to calculate the root value.

I am going to implement the second paper shortly. Surskit already has a class of randomly generated PI-SM games ("random-trees") ready to go. Although the discussion in Bosansky notes that some games do not permit large savings when using AlphaBeta. The random trees in surskit have the payoffs at terminal edges chosen independently of each other, which means they are in this class of stingy games. I might tweak the generation a bit to reflect the discussion in Section 5 of Bosansky; Those games are much more natural and permit higher savings.

I feel that there may be an even better algorithm to be discovered, based solely on my feelings that there is probably a simpler way to explain the workings of alphabeta than I've seen in the explanations and previous papers. 

## Stockfish

Stockfish and other conventional chess algorithms use AlphaBeta pruning in a depth first search as the cornerstone of their search. They attempt to explore all nodes up to a certain depth `n` from the root node. AlphaBeta pruning means they won't have to explore all of them though. 

I have to admin my understanding of conventional chess engines is lacking, and I have a lot more reading of the chessprogammingwiki to do. I'm just going to post a back and forth I had recently on the LC0 discord.

Me:
> Does anyone know where I could find a high level explanation of stockfish's search? I know it uses alpha beta which sets some depth limit and calculates the minimax value of the root assuming estimates at the leaf nodes of the subtree. But surely then it increases the depth over time. How is this done?

Guy:
> in a nutshell, `while(!stop) { search(depth++); }`. results are stored in a hash table with replacement policy favoring higher depth

Me:
> this doesnt address how the subsequent search() calls exploit the ones before, which they must otherwise why wouldn't you just call search with a higher depth to begin with. And I'm assuming search() is just depth first with alpha beta pruning, right?

Guy:
> you can call it with a higher depth, but it either finishes the depth or it doesn't, partial results doesn't make any sense. 
to break early, you call it with increasing depth and results are shared with a hash table
which makes a higher depth search reuse previous results, as search is basically a recursion of itself with current depth - X

Me:
> So assume that a move has been ruled out as bad by search with depth `n`. If then a search with higher depth `n'` is called it will still provide estimates for the new leaf nodes at full depth `n'` beyond that move? Even though the lower depth search suggests those nodes don't matter? I understand that you can't rely on that entirely since a move could only appear to be bad at low depth while actually being good. Still it seems you could still permit arbitrary depth search over the entire tree but still not check every subsequent leaf node at the full depth `n'`. Seems like it would more efficiently explore the tree this way. Fair enough though if it doesn't do any of that. Also it's not clear how hashed values at lower depths would be used in a higher depth search. Do you just reuse the same alpha, beta, and "v" values and pretend that they were calculated in the new search?

Guy:
> for every search(n), it starts with the root position, look at various information available(hash, history stats, etc) to make a ordered list of legal moves, then, starting from the best estimate, make the move, call search(n-1) on the next position, and continue, when the recursion returns, it writes current eval into the hash, and higher depth results overwrites lower ones
when n <=0, it goes into qsearch, or you can just consider it a static eval of the position
how many moves to look at is alpha beta, plus more pruning methods like it may search with n-x(x>1) or skip it entirely(pruning)
so in other words, with every higher depth search, it just repeatedly looking at the same stuff but with new alpha beta bounds and pruning rules, while leaf nodes will also end further

## Move Ordering

The amount of savings in AlphaBeta is very sensitive to the order in which moves are explored. You want to explore the best moves first so that you can rule out worse moves when encountered later. It seems this ordering is some of the information that is memoized between `search(n)` calls from the previous discussion.

## Optimizing the Stochastic Case

There is low hanging fruit in regards to optimizing AlphaBeta even more in the case of stochastic games. Naively adapting the algorithms above mean that we calculate the value for every matrix node that a chance node points to, so then the value of the chance node could be used in the matrix node above it.
However, can reason that if we've explored most of the chance node (that is some majority of its total probability) we may find that there are no values the remaining matrix nodes could have that would affect our calculation. Thus we can prune that chance node.
This requires knowing the probability of chance node edges/transitions, obviously. But clearly we need to know those to do AlphaBeta to begin with.

## Viability

All this to say that I think depth-first AlphaBeta is totally worth considering once we have an eval function to power it.
It seems the algorithm in Bosanky does compute some NE strategies so it could potentially power AlphaZero (as it then provides policy targets too). However it's still probably not suited for A0 since I believe you need to visit many more nodes with AlphaBeta.

# Surskit

Surskit is currently limited to implementing various MCTS but in a way that is modular, so different games ("states"), evaluation methods ("models") and bandit algorithms ("algorithm") can be swapped around without having to write any extra code. This can be seen in some code I wrote to compare the strength of Pasy's nascent heuristic function with pure monte carlo:

    using BattleSurskit = BattleSurskitVector<MaxTrace>;

    using MonteCarlo = MonteCarloModel<BattleSurskit>;
    using Heuristic = BattleHeuristic<BattleSurskit>;

    using Exp3pMonteCarlo = Exp3p<MonteCarlo, TreeBandit>;
    using MatrixUCBMonteCarlo = MatrixUCB<MonteCarlo, TreeBandit>;

    using Exp3pHeuristic = Exp3p<Heuristic, TreeBandit>;
    using MatrixUCBHeuristic = MatrixUCB<Heuristic, TreeBandit>;

First of all the battle wrapper has an templated log length (still called trace) to reduce memory use and speed up comparisons when we know the battle cannot produce a full length log. I also have a wrapper that uses std::arrays instead of std::vectors, so I could make a one line swap to that instead. Or i could change the precision of the real number representations from double to single... Modularity!
The next two lines define the models on this state, again so that changing the state will not affect this code.
The last four lines are the search algorithms, and notice the `TreeBandit` parameter. This is a curiously recurring template pattern that will use a single threaded tree search in the above code, but will use multi threaded search if I provide the classes `TreeBanditThreaded` or `TreeBanditThreadPool` instead.

Finally any two of these final 4 algorithm types are template parameters of the following class.
It's sole method simply forces these two algorithms to play versus eachother from an initial state/battle until it's terminal.


	template <class RowAlgorithm, class ColAlgorithm>
	class Vs
	{
	public:
	    using Real = typename RowAlgorithm::Types::Real;
	    using RealVector = typename RowAlgorithm::Types::VectorReal;
	    using RowModel = typename RowAlgorithm::Types::Model;
	    using ColModel = typename ColAlgorithm::Types::Model;
	    using State = typename RowAlgorithm::Types::State;

	    prng row_device;
	    prng col_device;

	    RowAlgorithm row_session;
	    ColAlgorithm col_session;

	    Vs() {}

	    Vs(int row_seed, int col_seed) : row_device(row_seed), col_device(col_seed)
	    {
	    }

	    Vs(RowAlgorithm &row_session, ColAlgorithm &col_session) : row_session(row_session), col_session(col_session) {}

	    void run (
	        int playouts,
	        State &state, 
	        RowModel &row_model, 
	        ColModel &col_model,
	        int &game_length    
	    ) {
	        state.get_actions();
	        game_length = 0;
	        while (!state.is_terminal) {
	            RealVector row_strategy(state.actions.rows), col_strategy(state.actions.cols);

	            MatrixNode<RowAlgorithm> row_root;
	            row_session.run(playouts, row_device, state, row_model, row_root);
	            row_session.get_strategies(&row_root, row_strategy, col_strategy);
	            int row_idx = row_device.sample_pdf(row_strategy, state.actions.rows);
	            auto row_action = state.actions.row_actions[row_idx];


	            MatrixNode<ColAlgorithm> col_root;
	            col_session.run(playouts, col_device, state, col_model, col_root);
	            col_session.get_strategies(&col_root, row_strategy, col_strategy);
	            int col_idx = col_device.sample_pdf(col_strategy, state.actions.cols);
	            auto col_action = state.actions.col_actions[col_idx];

	            state.apply_actions(row_action, col_action);
	            state.get_actions();
	            // std::cout << row_idx << ' ' << col_idx << std::endl;
	            ++game_length;
	        }
	    }
	};

The run function really only needs the battle and number of playouts per turn as its parameters. It could simply initialize the model within the function since all the current models can be default initialized. Also note the `prng` type. Any time randomness is used (like in every single bandit algorithm and in the rollout method of the pure monte carlo model) I force a `prng` 'device' to be used. Thus surskit is deterministic which I just think is neat. Although multithreaded search is obviously not deterministic...

# Development Directions

The next step is to create a model/evaluator for battles. There are several approaches being tried simultaneously.

* Heuristic
	Pasy is working on a fast heuristic that provides a value estimate. Its output can be bounded [0, 1] with a tanh activation function so it is more compatible with my MCTS searches. There, the value represents expected score where victory=1 and loss=0.
	This effort requires insight into battling to define the features (think piece values and king safety in chess engines). I think he's probably one of the best players to do this since he's a tournament level player and knows C++.

* AlphaZero
This is the approach I want to take personally because of my experience with the algorithm and RL as a whole. This method can work right now even without the `-Dchance` flag.

* AlphaBeta
This method is fast because it only visits each node once and uses game theoretic reasoning to compute the root payoff and strategies. However this efficiency means it needs to know the probability of transitions to propagate values down the tree since it does not sample like MCTS does. Again, implementing this is a high priority for me and I don't need your flag to be done to do so.

* Search Hyperparameter Tuning and Comparison
MatrixUCB and adversarial bandit algorithms each have hyperparameters that are tunable. The authors and AlphaZero and the LeelaChess devs agree that they are very important for strong search and RL. These parameters I think mostly depend on the game/domain and not the model, so there is cause to try to tune them now even without a strong model. I also have MatrixPUCB which is the SM analog to PUCT, the NN compatible bandit algorithm that was used in AlphaZero's MCTS.

## Goals

* **Stonger than human play in the perfect info setting**. This is the biggest goal that is probably easily attainable. I think the setup would be a bit like how heads up no-limit poker agents were shown to be super human. Many games vs the best players. In poker, the players were compensated based on how well they performed. I woudn't mind partially funding a exposition tournament if I believed in my baby.
* A tactics trainer utility that can scan games for states where there is a line of best play that is difficult to find. This tool would be a boon for the community. Even positions with mixed NE could be candidates as a way to teach players about 50/50s and finetune their sense of when its actually a 75/25, etc
* Analysis Engines. When a battle leaks enough info that a PI search can be used, this could be used by players after the fact to learn what the best moves were. Or even during the game :eyes:
* Faster DeepNash. I've implemented DeepNash (and spktrm in your discord has been continuing to work on it since) but I abandoned it because PS just runs too slow on my machine.
* Teambuilding. A big part of teambuilding is the hidden info aspect but there is still value in building strong teams for the perfect info game. They would clearly still be effective if used in the imperfect info setting too. I have well founded ideas about how to do this but I will leave it off this doc. Its already long enough!
