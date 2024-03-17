    #include <iostream>
    #include <vector>
    #include <string>
    #include <unordered_map>
    #include <algorithm>
    #include <cstdlib> // for rand() and srand()
    #include <ctime>   // for time()

    // Define enum for resources
    enum class Resource { Temple, Market, Farm, Settlement, Treasure };

    class Tile;
    class Kingdom;
    class Player;
    class Game;

    // Define class for a kingdom
    class Kingdom {
    public:
        Kingdom(std::string n) : name(n) {}

        std::string getName() const { return name; }

    private:
        std::string name;
    };

    // Define class for a tile
    class Tile {
public:
    std::vector<Resource> resources;
    Tile(std::vector<Resource> res, Kingdom* k, int r, int c)
        : resources(res), kingdom(k), row(r), col(c) {}

    std::vector<Resource> getResources() const { return resources; }
    Kingdom* getKingdom() const { return kingdom; }
    int getRow() const { return row; }
    int getCol() const { return col; }
    
    // Function to check if the tile has treasure
    bool hasTreasure() const {
        return std::find(this->resources.begin(), this->resources.end(), Resource::Treasure) != this->resources.end();
    }
     // Fixed typo here
    Kingdom* kingdom;
    int row;
    int col;
};

    // Define class for the game board
    class Board {
    public:
        Board(int rows, int cols) : numRows(rows), numCols(cols) {
            tiles.resize(numRows, std::vector<Tile*>(numCols, nullptr));
        }

        bool placeTile(int row, int col, Tile* tile) {
            if (row < 0 || row >= numRows || col < 0 || col >= numCols) {
                std::cout << "Invalid tile placement.\n";
                return false;
            }
            if (tiles[row][col] != nullptr) {
                std::cout << "Tile already exists at this position.\n";
                return false;
            }
            tiles[row][col] = tile;
            return true;
        }

        std::vector<std::vector<Tile*>> getTiles() const { return tiles; }

    private:
        int numRows;
        int numCols;
        std::vector<std::vector<Tile*>> tiles;
    };

    // Define class for a player
    class Player {
    public:
        Player(std::string n) : name(n), score(0) {}

        void addToScore(int points) {
            score += points;
        }

        std::string getName() const { return name; }
        int getScore() const { return score; }

    private:
        std::string name;
        int score;
    };

    // Define class for the game
    class Game {
    public:
        Game() : board(5, 5), currentPlayerIndex(0) {
            // Seed the random number generator for treasure selection
            srand(time(nullptr));

            players.push_back(Player("Player 1"));
            players.push_back(Player("Player 2"));
            redKingdom = new Kingdom("Red");
            blueKingdom = new Kingdom("Blue");
        }

        ~Game() {
            delete redKingdom;
            delete blueKingdom;
        }

        void play() {
            while (!isGameOver()) {
                Player& currentPlayer = players[currentPlayerIndex];
                std::cout << "It's " << currentPlayer.getName() << "'s turn.\n";

                // Player actions
                for (int i = 0; i < 2; ++i) {
                    placeTile(currentPlayer);
                }

                // Draw tiles to replenish player's hand
                drawTiles(currentPlayer);

                currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
            }

            calculateFinalScores();
            std::cout << "Game over! Final scores:\n";
            for (const auto& player : players) {
                std::cout << player.getName() << ": " << player.getScore() << std::endl;
            }
        }

    private:
        Board board;
        std::vector<Player> players;
        int currentPlayerIndex;
        Kingdom* redKingdom;
        Kingdom* blueKingdom;

        bool isGameOver() const {
        // Check if only one or two treasures remain on the board
        int treasureCount = countTreasures();
        if (treasureCount == 1 || treasureCount == 2) {
            return true;
        }

        // Check if the bag is empty and a player is unable to draw a tile
        if (isBagEmpty() && unableToDrawTile()) {
            return true;
        }

        return false;
        }

        int countTreasures() const {
        int count = 0;
        for (const auto& row : board.getTiles()) {
            for (const auto& tile : row) {
                if (tile != nullptr && hasTreasure(tile)) {
                    count++;
                }
            }
        }
        return count;
        }

        bool hasTreasure() const {
        return std::find(resources.begin(), resources.end(), Resource::Treasure) != resources.end();
        }


        bool containsTreasure(Tile* tile) const {
            // Implement logic to check if the tile contains a treasure
            // For example, you might have a function in the Tile class like hasTreasure()
            return tile->hasTreasure();
        }

        bool isBagEmpty() const {
            // Implement logic to check if the bag is empty
            // For example, if you have a vector representing the tile bag, check if it's empty
            return tileBag.empty();
        }

        bool unableToDrawTile() const {
            // Implement logic to check if a player is unable to draw a tile
            // For example, if you have a vector representing the tile bag, check if it's empty
            // Also, check if any player's hand is already full
            // You may need additional member variables to keep track of player hands and the tile bag
            return tileBag.empty() || playerHandIsFull();
        }


        void calculateFinalScores() {
            std::unordered_map<Kingdom*, int> kingdomCounts;
            for (const auto& player : players) {
                for (const auto& tile : board.getTiles()) {
                    for (const auto& rowTile : tile) {
                        if (rowTile != nullptr) {
                            Kingdom* tileKingdom = rowTile->getKingdom();
                            if (kingdomCounts.find(tileKingdom) == kingdomCounts.end()) {
                                kingdomCounts[tileKingdom] = 1;
                            } else {
                                kingdomCounts[tileKingdom]++;
                            }
                        }
                    }
                }
            }

            for (auto& player : players) {
                int leastCount = std::min(kingdomCounts[redKingdom], kingdomCounts[blueKingdom]);
                player.addToScore(-leastCount);
            }
        }

        
        // Placeholder function for placing a tile during player's turn
    void placeTile(Player& currentPlayer) {
        // Prompt the player to choose a row and column to place the tile
        int row, col;
        std::cout << "Enter the row and column to place the tile (e.g., 1 2): ";
        std::cin >> row >> col;

        // Create a new tile (for simplicity, just using a single resource)
        Tile* tile = new Tile({Resource::Temple}, redKingdom, row, col);

        // Place the tile on the board
        if (board.placeTile(row, col, tile)) {
            std::cout << currentPlayer.getName() << " placed a tile at (" << row << ", " << col << ").\n";
            
            // Check for conflicts and handle them
            handleConflicts(tile, currentPlayer);

            // Check for monument creation and handle it
            handleMonumentCreation(tile, currentPlayer);
        } else {
            std::cout << "Invalid tile placement. Please try again.\n";
            placeTile(currentPlayer); // Allow the player to try again
        }
    }

    // Function to handle conflicts when a tile is placed
    void handleConflicts(Tile* placedTile, Player& currentPlayer) {
        // Check for conflicts in neighboring tiles
        // For simplicity, let's assume each tile can have up to four neighbors (left, right, up, down)
        int row = placedTile->getRow();
        int col = placedTile->getCol();

        // Get neighboring tiles
        std::vector<Tile*> neighbors;
        if (row > 0 && board.getTiles()[row - 1][col] != nullptr)
            neighbors.push_back(board.getTiles()[row - 1][col]); // Up neighbor
        if (row < board.getTiles().size() - 1 && board.getTiles()[row + 1][col] != nullptr)
            neighbors.push_back(board.getTiles()[row + 1][col]); // Down neighbor
        if (col > 0 && board.getTiles()[row][col - 1] != nullptr)
            neighbors.push_back(board.getTiles()[row][col - 1]); // Left neighbor
        if (col < board.getTiles()[0].size() - 1 && board.getTiles()[row][col + 1] != nullptr)
            neighbors.push_back(board.getTiles()[row][col + 1]); // Right neighbor

        // Check for conflicts with neighboring tiles
        for (Tile* neighbor : neighbors) {
            if (neighbor->getKingdom() == placedTile->getKingdom() && /* Check if same kingdom */
                std::find(neighbor->getResources().begin(), neighbor->getResources().end(), Resource::Temple) != neighbor->getResources().end()) {
                // Conflict detected
                std::cout << "Conflict detected between tiles.\n";

                // Resolve conflict (for simplicity, assume current player wins)
                std::cout << currentPlayer.getName() << " wins the conflict.\n";
                // Remove opponent's leader and update their score accordingly
                // You may need to add additional functions to Player class to handle leaders and scores
            }
        }
    }


    // Function to handle monument creation
void handleMonumentCreation(Tile* placedTile, Player& currentPlayer) {
    // Check if a 2x2 grid of the same color tiles is formed
    int row = placedTile->getRow();
    int col = placedTile->getCol();

    // Check if the placed tile forms a 2x2 grid of the same color
    if (row > 0 && col > 0 &&
        board.getTiles()[row - 1][col] != nullptr && 
        board.getTiles()[row][col - 1] != nullptr &&
        board.getTiles()[row - 1][col - 1] != nullptr &&
        board.getTiles()[row - 1][col]->getKingdom() == placedTile->getKingdom() &&
        board.getTiles()[row][col - 1]->getKingdom() == placedTile->getKingdom() &&
        board.getTiles()[row - 1][col - 1]->getKingdom() == placedTile->getKingdom()) {
        
        // Create a monument of the same color
        Resource monumentType;

        // Prompt the user to choose the resource type for the monument
        std::cout << "A 2x2 grid is formed! Choose the resource type for the monument (Temple, Market, Farm, Settlement): ";
        
        // Read the input for monument type
        std::string input;
        std::cin >> input;

        // Convert the input to lowercase for case-insensitive comparison
        std::transform(input.begin(), input.end(), input.begin(), ::tolower);

        // Convert the input to the corresponding enum value
        if (input == "temple") {
            monumentType = Resource::Temple;
        } else if (input == "market") {
            monumentType = Resource::Market;
        } else if (input == "farm") {
            monumentType = Resource::Farm;
        } else if (input == "settlement") {
            monumentType = Resource::Settlement;
        } else {
            std::cerr << "Invalid resource type. Please choose from Temple, Market, Farm, or Settlement.\n";
            return; // Return without creating a monument if input is invalid
        }

        // Update player's score and handle other monument-related actions
        currentPlayer.addToScore(/* Points awarded for creating a monument */);

        // Update board to reflect monument creation
        // You need to implement this part to properly reflect the monument on the board
    }
}



        // Function to draw tiles to replenish player's hand
        void drawTiles(Player& currentPlayer) {
            // Determine how many tiles the player needs to draw to reach six tiles in hand
            int tilesToDraw = 6 - currentPlayer.getHandSize(); // Implement getHandSize() function as needed

            // Draw tiles from the tile bag
            for (int i = 0; i < tilesToDraw; ++i) {
                // Simulate drawing a tile from the tile bag
                // For demonstration, assume we just add a placeholder tile to the player's hand
                currentPlayer.addTileToHand(/* Placeholder tile */);
            }
        }
    };

    int main() {
        Game game;
        game.play();
        return 0;
    }
