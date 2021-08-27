# CPoker Clusters

CPoker is an extension of the Two Plus Two hand evaluator that lends itself to clustering poker hands after the flop and after the turn.
In order to use the file one must have already generated the handranks.dat file from the two plus two evaulator from the XPokerEval git repository.

Once that is done the function within poker.h called iterate_one_private_hand will output a binary file for each hand you give it with the amount of losses that hand would have against each board and hand combination. Ex. cards 1 and 2 would output 2118760 4 byte integers with the corresponding amout of losses against each of the 2118760 board combinations (50 choose 5) iterating through each hand that they could go up against, these files will be placed in /ehs.

My application requires that you do this manually by using iterate_all_private_hands which will return a vector of all possible private hands. You then pass each private hand to the iterate_one_private_hand and it will output the file for you. The files are too large for me to pass which is why this is necessary.

Once this is complete you can use the clustering functions to find your desired clusters. Adjust the amount of steps you want as necessary in order to reduce run time.

Overview of functions mentions:

vector<private_hands> iterate_all_private_hands(bool cards_used[]). This takes in a 52 bool array with true marked if that card is already being used and returns a vector of all possible private hands.

void iterate_one_private_hand(private_hands hand, int first_card, int second_card). This takes a private hand the integer of the first and second card and outputs the file in ./ehs directory with name 'first_card second_card.dat'.

void write_turn_clusters(int sample_size, int steps). This takes in the sample size (I used around 100000) for batch k means custering and the steps you want to take. Writes a file within ./Clusters directory of 200 clusters with 51 points for each in binary.

void write_flop_clusters(int sample_size, int steps). This takes in the sample size (I used around 100000) for batch k means custering and the steps you want to take. Writes a file within ./Clusters directory of 200 clusters with 51 points for each in binary.

