#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <string>

class GridSystem {
public:
	struct Node {
		glm::vec3 position;
		bool selected = false;
		int gCost;
		int hCost;
		int x;
		int y;
		bool walkable = true;
		Node* parent = nullptr;

		int fCost() { return gCost + hCost; }
	};

	std::vector<std::vector<Node>> nodes;
	int gridSizeX = 20;
	int gridSizeY = 20;
	float cellSize = 0.5f;

	std::set<Node*> path;

	std::shared_ptr<VertexBuffer> quadVertexBuffer;
	std::shared_ptr<IndexBuffer> quadIndexBuffer;
public:
	GridSystem() {
		for (int x = 0; x < gridSizeX; x++) {
			std::vector<Node> temp;
			for (int y = 0; y < gridSizeY; y++) {
				if (y > 3 && y < 16 && x > 3 && x < 16) {
					temp.push_back({ glm::vec3(x * 0.5f, 0, y * 0.5f), false, 0, 0, x, y, false });
				} else {
					temp.push_back({ glm::vec3(x * 0.5f, 0, y * 0.5f), false, 0, 0, x, y });
				}
			}
			nodes.push_back(temp);
		}
	}

	void selectNode(glm::vec3 pos) {
		for (auto& it : nodes) {
			for (auto& node : it) {
				node.selected = false;
			}
		}

		for (auto& it : nodes) {
			for (auto& node : it) {
				glm::vec3 diff = pos - node.position;
				if (abs(diff.x) < 0.5f && abs(diff.z) < 0.5f) {
					node.selected = true;
					return;
				}
			}
		}
	}

	void findPath(glm::vec3 startPos, glm::vec3 targetPos)
	{
		Node* startNode = nodeFromWorldPos(startPos);
		Node* targetNode = nodeFromWorldPos(targetPos);
		if (targetNode == nullptr) {
			return;
		}

		std::list<Node*> openSet;
		std::set<Node*> closeSet;

		openSet.push_back(startNode);

		while (openSet.size() > 0) {
			Node* current = openSet.front();
			for (auto& it : openSet) {
				if (it->fCost() < current->fCost() || it->fCost() == current->fCost()) {
					if (it->hCost < current->hCost) {
						current = it;
					}
				}
			}

			openSet.remove(current);
			closeSet.insert(current);

			if (current == targetNode) {
				retracePath(startNode, targetNode);
				return;
			}

			for (auto& it : getNeighbours(current)) {
				if (!it->walkable || closeSet.find(it) != closeSet.end()) {
					continue;
				}

				int newMovementCostToNeighbour = current->gCost + getDistance(current, it);
				if (newMovementCostToNeighbour < it->gCost || !contains(openSet, it)) {
					it->gCost = newMovementCostToNeighbour;
					it->hCost = getDistance(it, targetNode);
					it->parent = current;

					if (!contains(openSet, it)) {
						openSet.push_back(it);
					}
				}
			}
		}
	}

	void retracePath(Node* startNode, Node* targetNode) {
		std::set<Node*> path;

		Node* current = targetNode;
		while (current != startNode) {
			path.insert(current);
			current = current->parent;
		}

		this->path = path;
	}

	bool contains(std::list<Node*> list, Node* node) {
		for (auto& it : list) {
			if (it == node) {
				return true;
			}
		}

		return false;
	}

	Node* nodeFromWorldPos(glm::vec3 pos)
	{
		for (auto& it : nodes) {
			for (auto& node : it) {
				glm::vec3 diff = pos - node.position;
				if (abs(diff.x) < (cellSize / 2) && abs(diff.z) < cellSize / 2) {
					return &node;
				}
			}
		}
		return nullptr;
	}

	std::list<Node*> getNeighbours(Node* node)
	{
		std::list<Node*> temp;

		for (int x = -1; x <= 1; x++) {
			for (int y = -1; y <= 1; y++) {
				if (x == 0 && y == 0) {
					continue;
				}

				int checkX = node->x + x;
				int checkY = node->y + y;

				if (checkX >= 0 && checkX < gridSizeX && checkY >= 0 && checkY < gridSizeY) {
					temp.push_back(&this->nodes[checkX][checkY]);
				}
			}
		}

		return temp;
	}

	int getDistance(Node* a, Node* b) {
		int dstX = abs(a->x - b->x);
		int dstY = abs(a->y - b->y);

		if (dstX > dstY) {
			return 14 * dstY + 10 * (dstX - dstY);
		} else {
			return 14 * dstX + 10 * (dstY - dstX);
		}
	}
};