
# Tic-Tac-Toe Reinforcement Learning Bot

## Description

This project is about training a neural network to play Tic-tac-toe using techniques from  
the field of reinforcement learning, specifically PPO (Proximal Policy Optimization),   
which is a policy gradient method that is developed as an easy to implement  
algorithm with the benefits of TRPO (Trust Region Policy Optimization). The  
neural network learns by repeatedly playing games against itself and through the clipped  surrogate objective function determines how it should change its weights and biases.

For more information on the previously mentioned RL algorithms, see:
- [PPO](https://arxiv.org/pdf/1707.06347)
- [TRPO](https://arxiv.org/pdf/1502.05477)

## 0. Table of contents

1. [Installation](#1-installation)
2. [Usage](#2-usage)
3. [Dependencies](#3-dependencies)
4. [License](#4-license)
5. [Contact](#5-contact)

### 1. Installation

To install, first make sure that you have a bash shell, 'make' and a C compiler installed

1. Clone the repository:  
`git clone https://github.com/sde-policygradient/TttRlBot.git`
2. CD into the directory:  
`cd <dir name>`
3. Setup file extensions:  
    - Run `make setup CONFFILENAME="<your config file name>"`
    - Check \<your config file name\> and modify variables LIBEXTSAVE and MAINEXTSAVE as needed to fit your system.
4. To finally build the program, run:  
`make build_main CC=<compiler> CFLAGS="<compiler flags>" LDFLAGS="<linker flags>`

### 2. Usage

After the installation, the executable will be in 'build'. Run the file with:  
`<executable path> <-s<save dir>> <-i<iterations>> <-d<delay>> [--draw]`

- \<save dir\>: directory the neural network will be saved to.
- \<iterations\>: number of iterations to train the network for.
- \<delay\>: delay in milliseconds between network moves
- [--draw] (optional): tells the program to draw the board.

For example:  
`build/main.elf -snetwork -i50 --draw`

### 3. Dependencies

1. For installation:
    - A compatible C compiler
    - 'make'

2. For execution:
    - None

### 4. License
The project is licensed under the MIT License. See [LICENSE](LICENSE.txt) for further information.

### 5. Contact

Contact us at:  
<tebappc@gmail.com>
