#include "poker.h"
#include "poker_cluster.h"
#include <random>
using namespace std;



//function to return a random num
//used for populating the batch k means process
long random_num(long min, long max) {
  random_device rd;
  default_random_engine generator(rd());
  uniform_int_distribution<long> distribution(min, max);
  return distribution(generator);
}


//adjustment of k++ initialization 
//takes in the number of datapoints the histogram array and how many clusters needed
vector<cluster> k_means_initialization(int num_data_points, int array[][51], int clusters) {
  vector<cluster> return_vec;
  //then have to pick a random point to be the first cluster
  long random_point = random_num(0, num_data_points);
  cluster first;
  for (int i = 0; i < 51; i++) first.points[i] = array[random_point][i];
  return_vec.push_back(first);


  //then have to generate distance metrics for x amount of random points and take the furthest away
  //have to iterate over this and push to the cluster for how ever many centroids you would like
  long long distance;
  int max_point;  
  long max_distance = 0;
  long second_rand;


//add a cluster while still needed
  while (return_vec.size() < clusters) {
    for (int i = 0; i < 1000; i++){
      max_distance = 0;
      random_point = random_num(0, num_data_points);
      for (cluster x : return_vec) {
        distance = abs(earth_movers_distance(x.points, array[random_point]));
        //instead of porb distribution just did a 25% chance - does not really do anythig
        if (distance > max_distance && random_num(0, 100) < 25){
          max_point = random_point;
          max_distance = distance;
        }
      }
    }
    int zeroes= 0;
    for (int i = 0; i < 51; i++) first.points[i] = array[max_point][i];
    for (int i = 0; i < 51; i++) if (first.points[i] == 0) zeroes++;
    if (zeroes == 51) cout << max_point << "fuck ";
    return_vec.push_back(first);
  }
  return return_vec;
}

//function to assign cluster based on closest
int assign_cluster(vector<cluster> clusters, int* hist) {
  //go through each cluster and determine which is the closest
  int max_cluster, max_distance = 1000000000, counter = 0;
  long long distance;
  
  for (cluster x : clusters) {
    distance = abs(earth_movers_distance(x.points, hist));
    if (distance < max_distance) {
      max_cluster = counter;
      max_distance = distance;
    }
    counter++;
  }
  return max_cluster;

}



vector<cluster> naive_k_means_pass(vector<cluster> clusters, int hist[][51], long num_data_points, int batch_size = 0){
  if (batch_size == 0) batch_size = num_data_points;
  //for the batch size assign the amount of batch data points to each cluster
  
  int point, cluster_location;
  int push;

  //have vectors for each cluster in order to set the averages later
  vector<vector<int *> > new_vecs;
  vector<int *> x;

  for (int i; i < clusters.size(); i++) new_vecs.push_back(x);

  //iterates through in order to set averages later
  for (int i = 0; i < batch_size; i++) {

    if (batch_size == num_data_points) point = i;
    else point = random_num(0, batch_size);


    cluster_location = assign_cluster(clusters, hist[point]);


    new_vecs[cluster_location].push_back(hist[point]);


  }

 //zeroes out the previous place before adding together and averaging a new central location
 // in each cluster find the average of datapoints within that cluster and reset the cluster

  int counter = 0;
  vector<cluster> new_clusters;
  cluster centroid;
  double aver;
  int old[51];
  int empty_clusters = 0;


  for (cluster x : clusters) {
    for (int i = 0; i < 51; i++) {
      old[i] = x.points[i];
      x.points[i] = 0;
    }

    for (int* individual_hist : new_vecs[counter]) {

      for (int i = 0; i < 51; i++){
        x.points[i] += individual_hist[i];
      }
      
    }
    if (new_vecs[counter].size() != 0){
      for (int i = 0; i < 51; i++){
      aver = x.points[i] / floor(new_vecs[counter].size());
      centroid.points[i] = aver;
     }
    }

    //to fill an empty cluster takes a random point from the cluster with the most points
    else {
      
      int *new_point = hist[random_num(0, batch_size - 1)];
      for (int i = 0; i < 51; i++){
        centroid.points[i] = new_point[i];
      }
    }

    
    
    new_clusters.push_back(centroid);
    counter++;
  }
  cout << "empty clusters " << empty_clusters << endl;


  return new_clusters;
}


//function to find how much has changed from the last cluster to this cluster
long distance(int total_clusters, vector<cluster> new_clusters, vector<cluster> old_clusters) {
  long total_distance = 0;
  for (int i = 0; i < total_clusters; i++) {
    total_distance += abs(earth_movers_distance(new_clusters[i].points, old_clusters[i].points));
  }
  return total_distance;
}


//generates a random sample of histograms in order to use for the k means process
//sample should depend on the number of clusters required
void generate_random_sample_hist_turn(int datapoints, int hist[][51], int sample = 10000) {
  int rand_num, rand_num2;
  bool cards_used[52] = {false};


  vector<private_hands> hands = iterate_all_private_hands(cards_used);
  vector<board> boards = iterate_all_turns(cards_used);
  private_hands rand_hand;
  board rand_board;
  int counter = 0;
  int new_hist[51];
  int zeroes;

    //finds random hands and boards combination and finds their hist
  while (counter < sample) {
    rand_num = random_num(0, 1325);
    rand_num2 = random_num(0, boards.size() - 1);
    rand_hand = hands[rand_num];
    rand_board = boards[rand_num2];


    //continues if a duplicate card
    if (rand_hand.cards[0] == rand_board.cards[0] || rand_hand.cards[1] == rand_board.cards[0]) continue;
    if (rand_hand.cards[0] == rand_board.cards[1] || rand_hand.cards[1] == rand_board.cards[1]) continue;
    if (rand_hand.cards[0] == rand_board.cards[2] || rand_hand.cards[1] == rand_board.cards[2]) continue;
    if (rand_hand.cards[0] == rand_board.cards[3] || rand_hand.cards[1] == rand_board.cards[3]) continue;
    
    //puts the hist in the new_hist  which will then be put in the main 2d array later
    return_hist_turn(rand_hand.cards, rand_board.cards, new_hist);

    //error checking
    zeroes = 0;
    for (int i = 0; i < 51; i++) if (new_hist[i]== 0) zeroes++;
    if (zeroes > 50) {
      cout << rand_hand.cards[0] << " " << rand_hand.cards[1] << endl;
      cout << rand_board.cards[0] << " " << rand_board.cards[1] << " " << rand_board.cards[2] << " " << rand_board.cards[3] << endl;
    }

    //puts in 2d array
    for (int i = 0; i < 51; i++) hist[counter][i] = new_hist[i];
    counter++;

  }
}

//same function and deal as above just with a different amount of cards on the board
void generate_random_sample_hist_flop(int datapoints, int hist[][51], int sample = 10000) {
  int rand_num, rand_num2;
  bool cards_used[52] = {false};

  vector<private_hands> hands = iterate_all_private_hands(cards_used);
  vector<board> boards = iterate_all_flops(cards_used);
  private_hands rand_hand;
  board rand_board;
  int counter = 0;
  int new_hist[51];
  int zeroes;

  while (counter < sample) {
    rand_num = random_num(0, 1325);
    rand_num2 = random_num(0, boards.size() - 1);
    rand_hand = hands[rand_num];
    rand_board = boards[rand_num2];


    if (rand_hand.cards[0] == rand_board.cards[0] || rand_hand.cards[1] == rand_board.cards[0]) continue;
    if (rand_hand.cards[0] == rand_board.cards[1] || rand_hand.cards[1] == rand_board.cards[1]) continue;
    if (rand_hand.cards[0] == rand_board.cards[2] || rand_hand.cards[1] == rand_board.cards[2]) continue;
    

    return_hist_flop(rand_hand.cards, rand_board.cards, new_hist);

    zeroes = 0;
    for (int i = 0; i < 51; i++) if (new_hist[i]== 0) zeroes++;
    if (zeroes > 50) {
      cout << rand_hand.cards[0] << " " << rand_hand.cards[1] << endl;
      cout << rand_board.cards[0] << " " << rand_board.cards[1] << " " << rand_board.cards[2];
    }

    for (int i = 0; i < 51; i++) hist[counter][i] = new_hist[i];
    counter++;

  }
}


//writes the clusters into a binary file with each of the 200 clusters having 51 double points
void write_flop_clusters(int sample_size, int steps) {

  InitTheArrays(false);
  auto hist1 = new int[sample_size][51];
  int hist2[51];
  generate_random_sample_hist_flop(2598960, hist1, sample_size);

  vector<cluster> x;
  vector<cluster> old_clusters;

  int* cluster_assign = new int[2598960];

  x = k_means_initialization(sample_size, hist1, 200);
  long total_distance;

  //go through samples
  for (int i = 0; i < steps; i++) {
    old_clusters = x;
    generate_random_sample_hist_flop(2598960, hist1, sample_size);
    x = naive_k_means_pass(x, hist1, 2598960, sample_size);
    total_distance = distance(200, x, old_clusters);
    if (i % 2 == 0) cout << total_distance << endl;
    
  }
  cout << endl;

  generate_random_sample_hist_flop(2598960, hist1, sample_size);
  int assigned[200] = {0};
  int rand_n;
  
  for (int i = 0; i < sample_size; i++) {
    rand_n = assign_cluster(x, hist1[i]);
    assigned[rand_n]++;
  }
  for (int i = 0; i < 200; i++) cout << assigned[i] << endl;

  cout << endl;


  //need to save the clusters but is not necessary to save the actual points as that would be a large file and can compute cluster in real time
  FILE* fout;
  fout = fopen("./Clusters/flop_clusters.dat", "wb");
  for (cluster x2 : x){
    for (int i = 0; i < 51; i++){
      fwrite(&x2.points[i], sizeof(double), 1, fout);
    }
    
  }
  fclose(fout);

}


//reads in the flop clusters if they have been written already
vector<cluster> init_flop_clusters() {

  double cluster_spots[200*51] = {0};
  FILE* fin;
  fin = fopen("./Clusters/flop_clusters.dat", "rb");
  fread(cluster_spots, sizeof(cluster_spots), 1, fin);
  fclose(fin);
  vector<cluster> x;
  cluster new_c;
  for (int i = 0; i < 200; i++) {
    for (int f = 0; f < 51; f++) {
      new_c.points[f] = cluster_spots[i * 51 + f];
    }
    x.push_back(new_c);
  }
  return x;
}



//writes the turn clusters as discuess above for flop just now with a different board size
void write_turn_clusters(int sample_size, int steps) {
  InitTheArrays(false);
  auto hist1 = new int[sample_size][51];
  int hist2[51];
  generate_random_sample_hist_turn(305377800, hist1, sample_size);

  vector<cluster> x;
  vector<cluster> old_clusters;

  int* cluster_assign = new int[305377800];

  x = k_means_initialization(sample_size, hist1, 200);
  long total_distance;

  //go through steps
  for (int i = 0; i < steps; i++) {
    old_clusters = x;
    generate_random_sample_hist_turn(305377800, hist1, sample_size);
    x = naive_k_means_pass(x, hist1, 305377800, sample_size);
    total_distance = distance(200, x, old_clusters);
    if (i % 10 == 0) cout << total_distance << endl;
    
  }
  cout << endl;

  generate_random_sample_hist_turn(305377800, hist1, sample_size);
  int assigned[200] = {0};
  int rand_n;
  
  for (int i = 0; i < sample_size; i++) {
    rand_n = assign_cluster(x, hist1[i]);
    assigned[rand_n]++;
  }
  for (int i = 0; i < 200; i++) cout << assigned[i] << endl;

  cout << endl;

  


  //need to save the clusters but is not necessary to save the actual points as that would be a large file and can compute cluster in real time
  FILE* fout;
  fout = fopen("./Clusters/turn_clusters.dat", "wb");
  for (cluster x2 : x){
    for (int i = 0; i < 51; i++){
      fwrite(&x2.points[i], sizeof(double), 1, fout);
    }
    
  }
  fclose(fout);

}


vector<cluster> init_turn_clusters() {

  double cluster_spots[200*51] = {0};
  FILE* fin;
  fin = fopen("./Clusters/turn_clusters.dat", "rb");
  fread(cluster_spots, sizeof(cluster_spots), 1, fin);
  fclose(fin);
  vector<cluster> x;
  cluster new_c;
  for (int i = 0; i < 200; i++) {
    for (int f = 0; f < 51; f++) {
      new_c.points[f] = cluster_spots[i * 51 + f];
    }
    x.push_back(new_c);
  }
  return x;
}


//function to assign each private hand to a cluster and output a binary file
//binary file will have 4 bit integers corresponding to the cluster assigned to aka bits 0-4 correspond to 1st possible hand
void write_private_hand_clusters() {
  init_pre_flop_hist();
  cout << pre_flop_hist[0][0] << endl;
  vector<cluster> x;
  vector<cluster> old_clusters;
  int cluster_location;


  x = k_means_initialization(1326, pre_flop_hist, 8);
  long total_distance;

  for (int i = 0; i < 150; i++) {
    old_clusters = x;
    x = naive_k_means_pass(x, pre_flop_hist, 1326);
    total_distance = distance(8, x, old_clusters);
    //if (i % 10 == 0) cout << total_distance << endl;
  }

  int cluster_assign[1326] = {0};
  for (int i = 0; i < 1326; i++){
    cluster_location = assign_cluster(x, pre_flop_hist[i]);
    cluster_assign[i] = cluster_location;
  }

  FILE* fout;
  fout = fopen("./Clusters/private_hand_clusters.dat", "wb");
  fwrite(cluster_assign, sizeof(cluster_assign), 1, fout);
  fclose(fout);
}

//do not see the need for a finer grained abstraction then how mnay hands you won this should be 100 clusters
int return_cluster_final(int* private_hand, int* board) {
  int x = access_array(private_hand, board);
  return (x / 13);
}