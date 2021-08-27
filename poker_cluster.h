#include "poker.h"
using namespace std;

//centroid will have 51 spots to compare because 1326/50 is an even number so easier to compute
struct cluster {
  double points[51];
};

unsigned short access_array(int* hand, int* board) {
  if (total_array[555555] != 238) {
    InitTheArrays(false);
  }
  long private_comb = find_combination_number_private(hand);
  long board_comb = find_combination_number(hand, board);

  return total_array[private_comb * 2118760 + board_comb];
}

//going to have a histogram of 51 buckets so computing earth movers based off of that
long long earth_movers_distance(double* a, int* b) {
  long long earth_moved_prev = 0;
  long long temp;
  long long total_earth_moved = 0;
  for (int i = 0; i < 51; i++) {
    temp = (a[i] + earth_moved_prev - b[i]);
    total_earth_moved += temp;
    earth_moved_prev = temp;
  }
  return total_earth_moved;
}

//returns the earth movers distance look on math overflow if need explanation
long long earth_movers_distance(double* a, double* b) {
  long long earth_moved_prev = 0;
  long long temp;
  long long total_earth_moved = 0;
  for (int i = 0; i < 51; i++) {
    temp = (a[i] + earth_moved_prev - b[i]);
    total_earth_moved += temp;
    earth_moved_prev = temp;
  }
  return total_earth_moved;
}





//function to populate a histogram passed by address before the flop
//hand will be private cards first to find the place in the array
void return_hist_pre_flop(int* private_hand, int* hist) {
  bool used_cards[52] = { false };
  used_cards[private_hand[0] - 1] = true;
  used_cards[private_hand[1] - 1] = true;
  unsigned short x;
  memset(hist, 0, 51*sizeof(int));

  vector<board> all_board_cards = iterate_all_board_cards(used_cards);

  //the array returns hands - winner so basically all the hands that you lost
  // for each array going to bucket by how close it is 
  for (board board_cards : all_board_cards) {
      x = access_array(private_hand,board_cards.cards);
      for (int i = 1; i < 52; i++) {
          if (x < i * 26) {
              hist[i - 1] += 1;
              break;
          }
      }
  }
}


//function to populate a histogram passed by address after the flop
//hand will be private cards first to find the place in the array
void return_hist_flop(int* private_hand, int* board, int* hist) {
    memset(hist, 0, 51*sizeof(int));
  bool used_cards[52] = { false };
  used_cards[private_hand[0] - 1] = true;
  used_cards[private_hand[1] - 1] = true;
  used_cards[board[0] - 1] = true;
  used_cards[board[1] - 1] = true;
  used_cards[board[2] - 1] = true;

  unsigned short x;

  vector<private_hands> last_two_cards = iterate_all_private_hands(used_cards);

  //the array returns hands - winner so basically all the hands that you lost
  // for each array going to bucket by how close it is 
  for (private_hands board_cards : last_two_cards) {
      board[3] = board_cards.cards[0];
      board[4] = board_cards.cards[1];
      x = access_array(private_hand, board);
      hist[x/26] += 1;
  }
}

//function to populate a histogram passed by address after the flop
//hand will be private cards first to find the place in the array
void return_hist_turn(int* private_hand, int* board, int* hist) {
  memset(hist, 0, 51*sizeof(int));
  bool used_cards[52] = { false };
  used_cards[private_hand[0] - 1] = true;
  used_cards[private_hand[1] - 1] = true;
  used_cards[board[0] - 1] = true;
  used_cards[board[1] - 1] = true;
  used_cards[board[2] - 1] = true;
  used_cards[board[3] - 1] = true;

  unsigned short x;

  vector<int> last_card = iterate_last_card(used_cards);

  //the array returns hands - winner so basically all the hands that you lost
  // for each array going to bucket by how close it is 

  for (int last : last_card) {
      board[4] = last;
      x = access_array(private_hand, board);
      for (int i = 1; i < 52; i++) {
          if (x < i * 26) {
              hist[i - 1] += 1;
              break;
          }
      }
  }

}







