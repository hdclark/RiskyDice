
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![LOC](https://tokei.rs/b1/gitlab/hdeanclark/RiskyDice)](https://gitlab.com/hdeanclark/RiskyDice)
[![Language](https://img.shields.io/github/languages/top/hdclark/RiskyDice.svg)](https://gitlab.com/hdeanclark/RiskyDice)
[![Hit count](http://hits.dwyl.io/hdclark/RiskyDice.svg)](http://hits.dwyl.io/hdclark/RiskyDice)


# About

This program a simple dice roll simulator for the original version of the game
**Risk**. It saves you having to roll dice. It can simulate single attacks
(i.e., 3v2, 3v1, 2v2, 2v1, 1v2, and 1v1) or entire battles (e.g., 15v17) by
repeatedly simulating single attacks.

It is meant to present a simplified view. If replacing having to tediously roll
dice for each attack, just hitting the space bar will present a simulation for
all six single attack configurations.

This program will also report statistics about single attacks and entire
battles, if desired. Statistics are generated by simulation 10k attacks/battles.
Expected losses, winners, and the win rate are reported.


# License and Copying

All materials herein which may be copywrited, where applicable, are. Copyright
2019 Hal Clark. See [LICENSE.txt] for details about the license.

All liability is herefore disclaimed. The person(s) who use this source and/or
software do so strictly under their own volition. They assume all associated
liability for use and misuse, including but not limited to damages, harm,
injury, and death which may result, including but not limited to that arising
from unforeseen or unanticipated implementation defects.


# Dependencies

SFML, CMake, and a C++ compiler.


# Installation

This project uses CMake. Use the usual commands to compile:

     $>  cd /path/to/source/directory
     $>  mkdir build && cd build/
     $>  cmake -DCMAKE_INSTALL_PREFIX=/usr ../

then either

     $>  make

or

     $>  make && sudo make install

The script `compile.sh` will perform the above, but will not install anything.
