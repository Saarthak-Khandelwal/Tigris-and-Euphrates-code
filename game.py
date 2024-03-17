import tkinter as tk
import random

class Resource:
    Temple = "Temple"
    Market = "Market"
    Farm = "Farm"
    Settlement = "Settlement"
    Treasure = "Treasure"

class Tile:
    def __init__(self, resources):
        self.resources = resources

    def has_treasure(self):
        return Resource.Treasure in self.resources

class Player:
    def __init__(self, name):
        self.name = name
        self.score = 0

class GameBoard(tk.Frame):
    def __init__(self, master, rows, cols, *args, **kwargs):
        super().__init__(master, *args, **kwargs)
        self.rows = rows
        self.cols = cols
        self.tiles = [[None] * cols for _ in range(rows)]
        self.create_board()

    def create_board(self):
        for i in range(self.rows):
            for j in range(self.cols):
                resources = [random.choice(list(Resource.__dict__.values())) for _ in range(random.randint(1, 3))]
                tile = Tile(resources)
                self.tiles[i][j] = tile
                tile_label = tk.Label(self, text=str(tile.resources), width=15, height=2, relief="raised", borderwidth=2)
                tile_label.grid(row=i, column=j)

class GameApp(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Tigris and Euphrates")
        self.geometry("800x600")
        self.board = GameBoard(self, rows=4, cols=4)
        self.board.pack(expand=True, fill="both")

if __name__ == "__main__":
    app = GameApp()
    app.mainloop()
