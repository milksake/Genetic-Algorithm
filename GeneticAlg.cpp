#include "GeneticAlg.h"
#include <algorithm>
#include <numeric>

GeneticAlg::GeneticAlg(const std::vector<Vector2> cities, int populationSize, float probMutation):
	nodes(cities), probMutation(probMutation), bestPath(cities.size())
{
	for (int i = 0; i < bestPath.size(); i++)
		bestPath[i] = i;

	minAptitud = aptitud(bestPath);

	// Set initial population as random paths
	for (int i = 0; i < populationSize; i++)
		population.push_back(getRandomPath());
}

float GeneticAlg::aptitud(const std::vector<int>& path)
{
	// Euclidian distance of the path
	float sum = 0;
	for (int i = 0; i < path.size() - 1; i++)
	{
		sum += nodes[path[i]].eucliDist(nodes[path[i + 1]]);
	}
	sum += nodes[path[0]].eucliDist(nodes[path[path.size() - 1]]);
	return sum;
}

std::vector<int> GeneticAlg::selection()
{
	/* TOURNAMENT SELECTION */
	int K = 2;
	auto minPop = population[rand() % population.size()];
	float min = aptitud(minPop);
	for (int i = 1; i < K; i++)
	{
		auto pop = population[rand() % population.size()];
		float v = aptitud(pop);
		if (v < min)
		{
			min = v;
			minPop = pop;
		}
	}
	return minPop;
}

void GeneticAlg::crossOver(const std::vector<int>& parent1, const std::vector<int>& parent2, std::vector<int>& child1, std::vector<int>& child2)
{
	for (int i = 0; i < parent1.size(); i++)
	{
		if (rand() % 2)
		{
			child1.push_back(parent2[i]);
			child2.push_back(parent1[i]);
		}
		else
		{
			child1.push_back(-1);
			child2.push_back(-1);
		}
	}
	int ind1 = 0, ind2 = 0;
	for (int i = 0; i < child1.size(); i++)
	{
		if (child1[i] == -1)
		{
			for (; std::find(child1.begin(), child1.end(), parent1[ind1]) != child1.end(); ind1++);
			child1[i] = parent1[ind1];
			for (; std::find(child2.begin(), child2.end(), parent2[ind2]) != child2.end(); ind2++);
			child2[i] = parent2[ind2];
		}
	}
}

void GeneticAlg::mutation(std::vector<int>& path)
{
	/* MUTACIÓN BASADA EN ORDEN */
	int index1 = rand() % path.size();
	int	index2 = rand() % path.size();
	if (index1 > index2)
		std::swap(index1, index2);

	int temp = path[index2];
	path.erase(path.begin() + index2);
	path.insert(path.begin() + index1, temp);
}

std::vector<int> GeneticAlg::getRandomPath()
{
	std::vector<int> path(nodes.size());
	for (int i = 0; i < nodes.size(); i++)
		path[i] = i;

	std::random_shuffle(path.begin(), path.end());
	return path;
}

std::vector<Vector2> GeneticAlg::getNodesFromPath(const std::vector<int>& path)
{
	// Get the position of the cities in a path
	std::vector<Vector2> n(nodes.size());
	for (int j = 0; j < path.size(); j++)
		n[j] = nodes[path[j]];
	return n;
}

void GeneticAlg::update()
{
	/* ALGORITMO GENÉTICO */

	std::vector<std::vector<int>> newPop;

	while (newPop.size() < population.size())
	{
		// SELECTION
		auto parent1 = selection();
		auto parent2 = selection();

		// CROSSOVER
		std::vector<int> child1, child2;
		crossOver(parent1, parent2, child1, child2);

		// MUTATION
		int chance = rand() % 100;
		if (chance < probMutation)
			mutation(child1);
		chance = rand() % 100;
		if (chance < probMutation)
			mutation(child2);

		newPop.push_back(child1);
		if (newPop.size() == population.size())
			break;
		newPop.push_back(child2);
	}

	population = newPop;

	// UPDATE BEST PATH IF NEEDED
	for (auto& path : newPop)
	{
		auto v = aptitud(path);
		if (v < minAptitud)
		{
			minAptitud = v;
			bestPath = path;
		}
	}
}
