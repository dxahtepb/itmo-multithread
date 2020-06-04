### OpenMPI installation guide for Linux

1. Download [OpenMPI sources set](https://download.open-mpi.org/release/open-mpi/v4.0/openmpi-4.0.3.tar.gz)
2. Unzip sources `gunzip -c openmpi-4.0.3.tar.gz | tar xf -`
3. Go to upzipped directory `cd openmpi-4.0.3`
4. Configure installation `./configure --prefix=/opt/openmpi-4.0.3`
5. Run make `make -j <number_of_jobs>` and make install `make install`
6. Add `/opt/openmpi-4.0.3/bin` to your `PATH` environment variable

For troubleshooting refer to [OpenMPI FAQ](https://www.open-mpi.org/faq/?category=building) (and StackOverflow c:)
