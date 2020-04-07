FROM gitpod/workspace-full

USER gitpod

RUN pyenv global 3.7.7
RUN pip install conan
RUN sudo apt-get update && sudo apt-get install valgrind
# RUN conan profile update settings.compiler.libcxx=libstdc++11 default

# Install custom tools, runtime, etc. using apt-get
# For example, the command below would install "bastet" - a command line tetris clone:
#
# RUN sudo apt-get -q update && \
#     sudo apt-get install -yq bastet && \
#     sudo rm -rf /var/lib/apt/lists/*
#
# More information: https://www.gitpod.io/docs/config-docker/
