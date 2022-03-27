#include <fstream>
#include <iostream>
#include <vector>

#define INPUT_FILE "input.txt"
#define OUTPUT_FILE "output.txt"

using namespace std;

vector<vector<bool>> matrix;

void eraseSpot(int row, int col) {

  if (!matrix[row][col]) return;

  matrix[row][col] = false;

  eraseSpot(row + 1, col);
  eraseSpot(row - 1, col);
  eraseSpot(row, col + 1);
  eraseSpot(row, col - 1);
}

void solve() {
  int n, m;
  cin >> n >> m;

  matrix = vector<vector<bool>>(n + 2);
  for (int i = 0; i < n + 2; i++) {
    matrix[i] = vector<bool>(m + 2, false);
  }

  for (int i = 1; i < n + 1; i++) {
    for (int j = 1; j < m + 1; j++) {
      bool el;
      cin >> el;
      matrix[i][j] = el;
    }
  }

  int res = 0;

  for (int i = 1; i < n + 1; i++) {
    for (int j = 1; j < m + 1; j++) {
      if (matrix[i][j]) {
        res++;
        eraseSpot(i, j);
      }
    }
  }

  cout << res;
}

int main() {

  ifstream file_in(INPUT_FILE);
  ofstream file_out(OUTPUT_FILE);

  cin.rdbuf(file_in.rdbuf());
  cout.rdbuf(file_out.rdbuf());

  solve();

  file_in.close();
  file_out.close();

  return 0;
}