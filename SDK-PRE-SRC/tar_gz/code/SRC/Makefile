TREE_TYPE = TWO_LEVEL_TREE
#TREE_TYPE = THREE_LEVEL_TREE
#TREE_TYPE = ONE_LEVEL_TREE

CC = gcc
IDIR = INCLUDE
ODIR = OBJ
CFLAGS = -O3 -I$(IDIR) -D$(TREE_TYPE) -g 
#-Wall 

_DEPS = GainType.h Hashing.h Heap.h LKH.h Segment.h Sequence.h \
        future_net.h read_info.h route.h write_result.h \

DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = Activate.o AddCandidate.o AddTourCandidates.o 					\
	   AdjustCandidateSet.o AllocateStructures.o Ascent.o				\
       Best2OptMove.o Best3OptMove.o Best4OptMove.o Best5OptMove.o     \
       Between.o Between_SL.o BridgeGain.o C.o          \
       ChooseInitialTour.o Connect.o CreateCandidateSet.o              \
       eprintf.o \
       Excludable.o Exclude.o FindTour.o Flip.o Flip_SL.o Flip_SSL.o   \
       FreeStructures.o                                    \
       Gain23.o GenerateCandidates.o 		               \
       GreedyTour.o Hashing.o Heap.o         					\
       IsCandidate.o              \
       LinKernighan.o       	       \
       Make2OptMove.o Make3OptMove.o Make4OptMove.o Make5OptMove.o     \
       MergeWithTour.o          \
       Minimum1TreeCost.o MinimumSpanningTree.o 						\
       OrderCandidateSet.o       \
       qsort.o Random.o ReadParameters.o           			\
       ReadProblem.o RecordBestTour.o 				\
       ResetCandidateSet.o SegmentSize.o Sequence.o      \
       SFCTour.o StoreTour.o SymmetrizeCandidateSet.o     				\
       TrimCandidateSet.o WriteCandidates.o WriteTour.o 				\
       future_net.o read_info.o route.o write_result.o 					\
       ShortestPathAlgorithm.o RouteAlgorithm.o exam_repeated.o 		\
       
             
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: 
	all clean

all:
	$(MAKE) future_net

future_net: $(OBJ) $(DEPS)
	$(CC) -o ../future_net $(OBJ) $(CFLAGS) -lm

clean:
	/bin/rm -f $(ODIR)/*.o ../future_net *~ ._* $(IDIR)/*~ $(IDIR)/._* 

