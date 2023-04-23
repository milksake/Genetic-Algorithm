#pragma once
#include <vector>
#include "Vector2.h"

class GeneticAlg
{
public:
	GeneticAlg(const std::vector<Vector2> cities, int populationSize, float probMutation);

	// Probabilidad de cruzamiento y mutación
	float probMutation;

	// Vector de ciudades (posiciones en pixeles) 
	const std::vector<Vector2> nodes;

	// Representación de un camino: std::vector<int>
	// donde cada int es el index de la ciudad en el vector nodes

	std::vector<int> bestPath; // self-explanatory
	float minAptitud;

	// Población actual (vector de caminos)
	std::vector<std::vector<int>> population;

	// Función de aptitud (distancia euclideana). Se le pasa un camino
	float aptitud(const std::vector<int>& path);

	// Operadores Genéticos
	std::vector<int> selection();
	void crossOver(const std::vector<int>& parent1, const std::vector<int>& parent2, std::vector<int>& child1, std::vector<int>& child2);
	void mutation(std::vector<int>& path);

	// Misc
	std::vector<int> getRandomPath();
	std::vector<Vector2> getNodesFromPath(const std::vector<int>& path);

	// Update (llamada cada frame)
	void update();
};