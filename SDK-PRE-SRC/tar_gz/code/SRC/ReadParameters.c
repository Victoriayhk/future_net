#include "LKH.h"

/*
 * The ReadParameters function reads the name of a parameter file from
 * standard input and reads the problem parameters from this file.
 *
 * All entries of the parameter file are of the form <keyword >= <value>
 * (or <keyword><whitespace><value>), where <keyword> denotes an alphanumeric
 * keyword and <value> denotes alphanumeric or numeric data. Keywords are not
 * case sensitive.
 *
 * The order of specifications in the file is arbitrary. The following
 * specification is mandatory.
 *
 * PROBLEM_FILE = <string>
 * Specifies the name of the problem file.
 *
 * Additional control information may be supplied in the following format:
 *
 * ASCENT_CANDIDATES = <integer>
 * The number of candidate edges to be associated with each node during
 * the ascent. The candidate set is complemented such that every candidate
 * edge is associated with both its two end nodes.
 * Default: 50.
 *
 * BACKBONE_TRIALS = <integer>
 * The number of backbone trials in each run.
 * Default: 0.
 *
 * BACKTRACKING = { YES | NO }
 * Specifies whether a backtracking K-opt move is to be used as the first
 * move in a sequence of moves (where K = MOVE_TYPE).
 * Default: NO.
 *
 * CANDIDATE_FILE = <string>
 * Specifies the name of a file to which the candidate sets are to be
 * written. If, however, the file already exists, the candidate edges are
 * read from the file. The first line of the file contains the number of
 * nodes. Each of the following lines contains a node number, the number of
 * the dad of the node in the minimum spanning tree
 * (0, if the node has no dad), the number of candidate edges emanating
 * from the node, followed by the candidate edges. For each candidate edge
 * its end node number and alpha-value are given.
 * It is possible to give more than one CANDIDATE_FILE specification. In this
 * case the given files are read and the union of their candidate edges is
 * used as candidate sets.
 *
 * CANDIDATE_SET_TYPE = { ALPHA | DELAUNAY [ PURE ] | NEAREST-NEIGHBOR |
 *                        QUADRANT }
 * Specifies the candidate set type.
 * ALPHA is LKH's default type. It is applicable in general.
 * The other four types can only be used for instances given by coordinates.
 * The optional suffix PURE for the DELAUNAY type specifies that only
 * edges of the Delaunay graph are used as candidates.
 * Default: ALPHA.
 *
 * COMMENT <string>
 * A comment.
 *
 * # <string>
 * A comment.
 *
 * EOF
 * Terminates the input data. The entry is optional.
 *
 * EXCESS = <real>
 * The maximum alpha-value allowed for any candidate edge is set to
 * EXCESS times the absolute value of the lower bound of a solution
 * tour (determined by the ascent).
 * Default: value of 1.0/DIMENSION.
 *
 * EXTRA_CANDIDATES = <integer> [ SYMMETRIC ]
 * Number of extra candidate edges to be added to the candidate set
 * of each node. Their candidate set type may be specified after the
 * keyword EXTRA_CANDIDATE_SET_TYPE.
 * The integer may be followed by the keyword SYMMETRIC, signifying
 * that these extra candidate edges is to be complemented such
 * that each of them is associated with both its two end nodes.
 * Default: 0
 *
 * EXTRA_CANDIDATE_SET_TYPE = { NEAREST-NEIGHBOR | QUADRANT }
 * The candidate set type of extra candidate edges.
 * Default: QUADRANT
 *
 * GAIN23 = { YES | NO }
 * Specifies whether the Gain23 function is used.
 * Default: YES.
 *
 * GAIN_CRITERION = { YES | NO }
 * Specifies whether Lin and Kernighan's gain criterion is used.
 * Default: YES.
 *
 * INITIAL_PERIOD = <integer>
 * The length of the first period in the ascent.
 * Default: value of DIMENSION/2 (but at least 100).
 *
 * INITIAL_STEP_SIZE = <integer>
 * The initial step size used in the ascent.
 * Default: 1.
 *
 * INITIAL_TOUR_ALGORITHM = { BORUVKA | GREEDY | MOORE | NEAREST-NEIGHBOR |
 *                            QUICK-BORUVKA | SIERPINSKI | WALK }
 * Specifies the algorithm for obtaining an initial tour.
 * Default: WALK.
 *
 * INITIAL_TOUR_FILE = <string>
 * Specifies the name of a file containing a tour to be used as the
 * initial tour in the search. The tour is given by a list of integers
 * giving the sequence in which the nodes are visited in the tour.
 * The tour is terminated by a -1.
 * See also INITIAL_TOUR_FRACTION.
 *
 * INITIAL_TOUR_FRACTION = <real in [0;1]>
 * Specifies the fraction of the initial tour to be constructed by means
 * of INITIAL_TOUR_FILE edges.
 * Default: 1.0
 *
 * INPUT_TOUR_FILE = <string>
 * Specifies the name of a file containing a tour. The tour is used to
 * limit the search (the last edge to be excluded in a non-gainful move
 * must not belong to the tour). In addition, the Alpha field of its
 * edges is set to zero. The tour is given by a list of integers giving
 * the sequence in which the nodes are visited in the tour. The tour is
 * terminated by a -1.
 *
 * KICK_TYPE = <integer>
 * Specifies the value of K for a random K-swap kick. If KICK_TYPE is
 * zero, then the LKH's special kicking strategy, WALK, is used.
 * Default: 0.
 *
 * KICKS = <integer>
 * Specifies the number of times to "kick" a tour found by Lin-Kernighan.
 * Each kick is a random K-swap-kick move. However, KICKS is zero, then
 * LKH's special kicking strategy, WALK, is used.
 * Default: 1.
 *
 * MAX_BREADTH = <integer>
 * The maximum number of candidate edges considered at each level of
 * the search for a move.
 * Default: INT_MAX.
 *
 * MAX_CANDIDATES = <integer> [ SYMMETRIC ]
 * The maximum number of candidate edges to be associated with each node.
 * The integer may be followed by the keyword SYMMETRIC, signifying
 * that the candidate set is to be complemented such that every candidate
 * edge is associated with both its two end nodes.
 * If MAX_CANDIDATES is zero the candidate sets are made up of the
 * edges represented in the CANDIDATE_FILEs, the INITIAL_TOUR_FILE,
 * the INPUT_TOUR_FILE, the SUBPROBLEM_TOUR_FILE, and the MERGE_TOUR_FILEs.
 * Default: 5.
 *
 * MAX_SWAPS = <integer>
 * Specifies the maximum number of swaps (flips) allowed in any search
 * for a tour improvement.
 * Default: value of DIMENSION.
 *
 * MAX_TRIALS = <integer>
 * The maximum number of trials in each run.
 * Default: number of nodes (DIMENSION, given in the problem file).
 *
 * MERGE_TOUR_FILE = <string>
 * Specifies the name of a tour to be merged. The edges of the tour are
 * added to the candidate sets.
 * It is possible to give more than two MERGE_TOUR_FILE specifications.
 *
 * MOVE_TYPE = <integer>
 * Specifies the sequential move type to be used as submove in Lin-Kernighan.
 * A value K >= 2 signifies that a sequential K-opt move is used.
 * Default: 5.
 *
 * NONSEQUENTIAL_MOVE_TYPE = <integer>
 * Specifies the nonsequential move type to be used. A value K >= 4
 * signifies that attempts are made to improve a tour by nonsequential
 * k-opt moves where 4 <= k <= K. Note, however, that the effect depends
 * on the specifications of PATCHING_C and PATCHING_A.
 * Default: value of (MOVE_TYPE + PATCHING_C + PATCHING_A - 1).
 *
 * OUTPUT_TOUR_FILE = <string>
 * Specifies the name of a file where the best tour is to be written.
 * Each time a trial has produced a new best tour, the tour is written
 * to this file.
 * The character '$' in the name has a special meaning. All occurrences
 * are replaced by the cost of the tour.
 *
 * OPTIMUM = <integer>
 * Known optimal tour length. If STOP_AT_OPTIMUM is YES, a run will be
 * terminated if the tour length becomes equal to this value.
 * Default: value of MINUS_INFINITY.
 *
 * PATCHING_A = <integer> [ RESTRICTED | EXTENDED ]
 * The maximum number of disjoint alternating cycles to be used for
 * patching. An attempt to patch cycles is made if the corresponding
 * non-sequential move is gainful.
 * The integer may be followed by the keyword RESTRICTED or EXTENDED.
 * The keyword RESTRICTED signifies that gainful moves are only
 * considered if all its inclusion edges are candidate edges.
 * The keyword EXTENDED signifies that the non-sequential move need
 * not be gainful if only all its inclusion edges are candidate edges.
 * Default: 1.
 *
 * PATCHING_C = <integer> [ RESTRICTED | EXTENDED ]
 * The maximum number of disjoint cycles to be patched in an attempt
 * to find a feasible and gainful move. An attempt to patch cycles is
 * made if the corresponding non-sequential move is gainful.
 * The integer may be followed by the keyword RESTRICTED or EXTENDED.
 * The keyword RESTRICTED signifies that gainful moves are only
 * considered if all its inclusion edges are candidate edges.
 * The keyword EXTENDED signifies that the non-sequential move need
 * not be gainful if only all its inclusion edges are candidate edges.
 * Default: 0.
 *
 * PI_FILE = <string>
 * Specifies the name of a file to which penalties (Pi-values determined
 * by the ascent) are to be written. If the file already exists, the
 * penalties are read from the file, and the ascent is skipped.
 * The first line of the file contains the number of nodes. Each of the
 * following lines is of the form
 *       <integer> <integer>
 * where the first integer is a node number, and the second integer is
 * the Pi-value associated with the node.
 * The file name "0" represents a file with all Pi-values equal to zero.
 *
 * POPULATION_SIZE = <integer>
 * Specifies the maximum size of the population in the genetic algorithm.
 * Default: 0.
 *
 * PRECISION = <integer>
 * The internal precision in the representation of transformed distances:
 *    d[i][j] = PRECISION*c[i][j] + pi[i] + pi[j],
 * where d[i][j], c[i][j], pi[i] and pi[j] are all integral.
 * Default: 100 (which corresponds to 2 decimal places).
 *
 * RESTRICTED_SEARCH = { YES | NO }
 * Specifies whether the following search pruning technique is used:
 * The first edge to be broken in a move must not belong to the currently
 * best solution tour. When no solution tour is known, it must not belong
 * to the minimum spanning 1-tree.
 * Default: YES.
 *
 * RUNS = <integer>
 * The total number of runs.
 * Default: 10.
 *
 * SEED = <integer>
 * Specifies the initial seed for random number generation. If zero, the
 * seed is derived from the system clock.
 * Default: 1.
 *
 * STOP_AT_OPTIMUM = { YES | NO }
 * Specifies whether a run is stopped, if the tour length becomes equal
 * to OPTIMUM.
 * Default: YES.
 *
 * SUBGRADIENT = { YES | NO }
 * Specifies whether the Pi-values should be determined by subgradient
 * optimization.
 * Default: YES.
 *
 * SUBPROBLEM_SIZE = <integer> [ DELAUNAY | KARP | K-CENTER | K-MEANS | MOORE |
 *                               ROHE | SIERPINSKI ] [ BORDERS ] [ COMPRESSED ]
 * The number of nodes in a division of the original problem into subproblems.
 * The division is made according to the tour given by SUBPROBLEM_TOUR_FILE.
 * The value 0 signifies that no division is made.
 * By default the subproblems are determined by subdividing the tour into
 * segments of equal size. However, the integer may be followed by DELAUNAY,
 * KARP, K-CENTER, K-MEANS, MOORE, ROHE or SIERPINSKI. DELAUNAY specifies that
 * the Delaunay partitioning scheme is used, KARP that Karp's partitioning
 * scheme is used, K-CENTER that a partitioning scheme based on K-center
 * clustering, K-MEANS that a partitioning scheme based on K-means clustering
 * is used, ROHE that Rohe's random rectange partitining scheme is used, and
 * MOORE or SIERPINSKI that a partitioning scheme based on either a Moore or
 * Sierpinski space-filling curve is used.
 * The BORDERS specification signifies that the subproblems along the borders
 * between subproblems are to be solved too.
 * The COMPRESSED specification signifies that each subproblem is compressed by
 * removing from the problem all nodes with two incident subproblem tour edges
 * that belong to all tours to be merged (at least two MERGE_TOUR_FILEs should
 * be given).
 * Default: 0.
 *
 * SUBPROBLEM_TOUR_FILE = <string>
 * Specifies the name of a file containing a tour to be used for dividing
 * the original problem into subproblems. The approximate number of nodes
 * in each is * given by SUBPROBLEM_SIZE.
 * The tour is given by a list of integers giving the sequence in which the
 * nodes are visited in the tour. The tour is terminated by a -1
 *
 * SUBSEQUENT_MOVE_TYPE = <integer>
 * Specifies the move type to be used for all moves following the first move
 * in a sequence of moves. The value K >= 2 signifies that a K-opt move is to
 * be used. The value 0 signifies that all moves are of the same type
 * (K = MOVE_TYPE).
 * Default: 0.
 *
 * SUBSEQUENT_PATCHING = { YES | NO }
 * Specifies whether patching is used for moves following the first move
 * in a sequence of moves.
 * Default: YES.
 *
 * TIME_LIMIT = <real>
 * Specifies a time limit in seconds for each run.
 * Default: value of DBL_MAX.
 *
 * TOUR_FILE = <string>
 * Specifies the name of a file where the best tour is to be written.
 * When a run has produced a new best tour, the tour is written to
 * this file.
 * The character '$' in the name has a special meaning. All occurrences
 * are replaced by the cost of the tour.
 *
 * TRACE_LEVEL = <integer>
 * Specifies the level of detail of the output given during the solution
 * process. The value 0 signifies a minimum amount of output. The higher
 * the value is the more information is given.
 * Default: 1.
 *
 * List of abbreviations
 * ---------------------
 *
 * A string value may be abbreviated to the first few letters of the string,
 * if that abbreviation is unambiguous.
 *
 *     Value        Abbreviation
 *     ALPHA             A
 *     BORDERS           B
 *     BORUVKA           B
 *     COMPRESSED        C
 *     DELAUNAY          D
 *     EXTENDED          E
 *     GREEDY            G
 *     KARP              KA
 *     K-CENTER          K-C
 *     K-MEANS           K-M
 *     MOORE             M
 *     NEAREST-NEIGHBOR  N
 *     NO                N
 *     PURE              P
 *     QUADRANT          Q
 *     QUICK-BORUVKA     Q
 *     RESTRICTED        R
 *     ROHE              R
 *     SIERPINSKI        S
 *     SYMMETRIC         S
 *     WALK              W
 *     YES               Y
 */

void ReadParameters()
{
	OutputTourFileName = TourFileName = 0;
    CandidateFiles = MergeTourFiles = 0;
    AscentCandidates = 50;
    //BackboneTrials = 0;
    Backtracking = 0;
    CandidateSetSymmetric = 0;
    CandidateSetType = ALPHA;
    //Gain23Used = 1;
    GainCriterionUsed = 1;//1还是0没有什么卵用（初赛上）
    InitialPeriod = -1;
    InitialStepSize = 1;
    InitialTourAlgorithm = WALK;//GREEDY、QUICK_BORUVKA、WALK
    //InitialTourFraction = 1.0;
    //Kicks = 1;
    //KickType = 0;
    MaxBreadth = INT_MAX;
    MaxCandidates = 10;
    MaxSwaps = -1;
	
    SubsequentMoveType = MoveType = 5;
	
    NonsequentialMoveType = -1;
    Optimum = MINUS_INFINITY;
    PatchingA = 1;
    PatchingC = 0;
    PatchingAExtended = 0;
    PatchingARestricted = 0;
    PatchingCExtended = 0;
    PatchingCRestricted = 0;
    Precision = 100;
    RestrictedSearch = 1;
    RohePartitioning = 0;
    Seed = 1;
    SierpinskiPartitioning = 0;
    StopAtOptimum = 1;
    Subgradient = 1;
    SubproblemBorders = 0;
    SubproblemsCompressed = 0;
    SubproblemSize = 0;
    SubsequentPatching = 1;
    TimeLimit = DBL_MAX;
    TraceLevel = 1;
}
