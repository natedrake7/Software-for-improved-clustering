lsh:
	@echo "Compiling lsh..."
	g++ -o lsh ./LSH/lsh.cpp ./LSH/hash.cpp ./LSH/lsh_main.cpp ./LSH/misc.cpp
	@echo "Execute with: ./lsh -d ./Data/input.dat -q ./Data/query.dat -o output.txt etc..."

cube:
	@echo "Compiling hypercube..."
	g++ -o cube ./HyperCube/hypercube_main.cpp ./HyperCube/hypercube.cpp ./LSH/lsh.cpp ./LSH/hash.cpp ./LSH/misc.cpp
	@echo "Execute with: ./cube -d ./Data/input.dat -q ./Data/query.dat -o output.txt etc..."
cluster:
	@echo "Compiling cluster..."
	g++ -o cluster ./Cluster/cluster_main.cpp ./Cluster/cluster.cpp ./LSH/lsh.cpp ./LSH/hash.cpp ./LSH/misc.cpp ./HyperCube/hypercube.cpp
	@echo "Execute with: ./cluster -i ./Data/input.dat -c cluster.conf -m LSH etc..."

clean:
	@echo "Cleaning up..."
	rm -f cluster lsh cube