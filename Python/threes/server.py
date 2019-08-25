#!/usr/bin/env python3
import logging
"""
https://en.wikipedia.org/wiki/Shithead_(card_game)
Currently unplayable
"""
import socket
from common import *
from collections import defaultdict
from random import shuffle
from deck import Card, Deck, Hand, byRank, netToCard
from itertools import starmap, takewhile, islice
from functools import partial

from threading import Thread, Barrier, Event, Lock
from queue import Queue
from enum import Enum, auto

""" rules TODO
turntaking
game start:
    9 cards each initially (3 per hand)
    switch primaries and faceups
A is high
card draw (fill to 3) while rest exist
played pile may only rise in rank
    unless burned by 4 chain or 10
    or reset by 2
    ELSE pickup played pile to hand
chaining rules:
    rank rises for same suit
    suit can change for same rank
    ^ either must hold throughout chain
held, faceup and faceown may only be accessed
in sequence as exhausted
"""


class Task(Enum):
    MSG = auto()
    ENDGAME = auto()
    SWAP = auto()
    WAIT = auto()


class Player(Thread):
    """client handler"""

    def __init__(self, deathevent: Event, sock, addr=NIX):
        self.pipe = Queue()
        self.sock = sock
        self.addr = addr
        self.deathevent = deathevent
        super().__init__()

    def run(self):
        with self.sock.makefile(mode='rw') as conn:
            terminate = False
            while not terminate or not self.deathevent.wait(0.5):

                try:
                    task, *args = self.pipe.get()
                    msg = ""
                    if task == Task.ENDGAME:
                        msg = f"{task.name} {args[0]}"
                        terminate = True
                    elif task == Task.MSG:
                        lines, msg = args
                        msg = f"{task.name} {lines}\n{msg}"
                    elif task == Task.SWAP:
                        self._swap(conn)
                    elif task == Task.WAIT:
                        args[0].wait()
                    conn.write(msg + '\n')
                    conn.flush()
                except:
                    self.deathevent.set()

    def showHand(self):
        """Display hand to user"""
        with datalock:
            self.msg(str(playerdata[self.ident]))

    def swap(self):
        """Request user swaps faceups"""
        self.pipe.put_nowait((Task.SWAP,))

    def _swap(self, conn):
        # NOTE, implementation doesn't have error conditions
        conn.write(Task.SWAP.name + '\n')
        conn.flush()
        stuff = conn.readline().strip().split(' ')
        cards = list(map(netToCard, stuff))
        froms, tos = cards[::2], cards[1::2]
        with datalock:
            for tup in zip(froms, tos):
                playerdata[self.ident].swap(*tup)  # you do stuff now though?
        conn.write(ACK + '\n')
        conn.flush()

    def msg(self, msg):
        """Send general information to user"""
        lines = len(msg.split('\n'))
        self.pipe.put_nowait((Task.MSG, lines, msg))

    def sort(self):
        """Sort the hand"""
        with datalock:
            playerdata[self.ident].sort()

    def endgame(self, winner):
        """Declare game finished, and winner"""
        self.pipe.put_nowait((Task.ENDGAME, winner))

    def wait(self, bar: Barrier):
        self.pipe.put_nowait((Task.WAIT, bar))


def broadcast(players, method, *args):
    """SEND to all players"""
    for player in players.values():
        method(player, *args)


def multicast(players, ks, method, *args, inclusive=True):
    """
    SEND to players in ks
    if inclusive==False, SEND to all except
    """
    if inclusive:
        def pred(k): return k not in ks
    else:
        def pred(k): return k in ks
    broadcast({k: v for k, v in players.items() if pred(k)}, method, *args)


def gameloop(players):
    """Main controller"""
    for player in players.values():
        print(f"Conn on: {player.addr or NIX}")
    turnkeys = list(players.keys())
    shuffle(turnkeys)
    # just terminate game for now, after showing hand
    bar = Barrier(len(players))
    broadcast(players, Player.wait, bar)
    broadcast(players, Player.showHand)
    broadcast(players, Player.swap)
    broadcast(players, Player.showHand)
    broadcast(players, Player.endgame, None)
    for player in players.values():
        player.join()


if __name__ == "__main__":
    from argparse import ArgumentParser, ArgumentTypeError

    def player_type(i):
        x = int(i)
        if not x > 1:
            raise ArgumentTypeError("Must have more than 1 player.")
        return x

    parser = ArgumentParser("Threes server")
    parser.add_argument("players", type=player_type,
                        help="Number of players to serve")
    parser.add_argument(
        "--local", action='store_const', default='inet', const='unix', help="Network over unix sockets")
    argv = parser.parse_args()

    server = socket.socket(trans_mode[argv.local][0])
    server.bind(*(trans_mode[argv.local][1]))
    try:
        server.listen()
        players = dict()
        global playpile
        global graveyard
        global playerdata
        global datalock
        datalock = Lock()
        playerdata = dict()
        graveyard = []
        hands, playpile = Deck.deal(argv.players, 9)
        hands = map(Hand, hands)
        deathevent = Event()

        for hand in hands:
            p = Player(deathevent, *server.accept())
            p.start()
            """
            performing the binding now could be safe
            because no commands are issued yet
            and a barrier must be passed first
            """
            playerdata[p.ident] = hand
            players[p.ident] = p

        gameloop(players)
    finally:
        if argv.local == 'unix':
            from pathlib import Path
            Path(NIX).unlink()
