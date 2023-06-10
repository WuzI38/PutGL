import chess
import chess.pgn
if __name__ == '__main__':
    pgn = open("game.pgn")
    game = chess.pgn.read_game(pgn)
    pgn.close()
    f = open("game.csv", "w")

    for move in game.mainline_moves():
        from_sqr = chess.square_name(move.from_square)
        to_sqr = chess.square_name(move.to_square)
        promotion = move.promotion
        if promotion is not None:
            promotion = chess.Piece(promotion, chess.WHITE).symbol().upper()
        else:
            promotion = '0'
        f.write(f"{from_sqr},{to_sqr},{promotion}\n")
    f.close()
    
        