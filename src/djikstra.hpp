#ifndef MALAISE_DJIKSTRA_HPP
#define MALAISE_DJIKSTRA_HPP

#include <algorithm>
#include <climits>
#include <functional>
#include <map>
#include <queue>
#include <vector>

#include "vec2i.hpp"

namespace malaise::algorithm {
	namespace {
		struct DjikstraNode {
			math::Vec2i position;
			int distance;

			bool operator>(const DjikstraNode& other) const {
				return distance > other.distance;
			}
				};

			std::vector<math::Vec2i> get_neighbors(const math::Vec2i& node, const std::vector<math::Vec2i>& walkable) {
				static const math::Vec2i directions[] = { // possible movement directions, no diagonals
					{ 1, 0 },
					{-1, 0 },
					{ 0, 1 },
					{ 0,-1 }
				};

				std::vector<math::Vec2i> result;

				for (auto direction : directions) {
					math::Vec2i candidate = node + direction;

					if (std::find(walkable.begin(), walkable.end(), candidate) != walkable.end())
						result.push_back(candidate);
				}

				return result;
			}
		}

	inline std::vector<math::Vec2i> djikstras_algorithm(const std::vector<math::Vec2i> &graph, const math::Vec2i& source, const math::Vec2i& target) {
		std::priority_queue<DjikstraNode, std::vector<DjikstraNode>, std::greater<>> frontier;

		std::map<math::Vec2i, int> distances;
		std::map<math::Vec2i, math::Vec2i> previous;

		for (const auto &node : graph) {
			distances[node] = INT_MAX;
		}

		distances[target] = 0;

		frontier.emplace(target, 0);

		while (!frontier.empty()) {
			auto current_node = frontier.top();
			frontier.pop();

			if (current_node.position == source) // found path from target to source
				break;

			for (auto neighbor : get_neighbors(current_node.position, graph)) {
				int tentative = distances[current_node.position] + 1;

				if (tentative < distances[neighbor]) {
					distances[neighbor] = tentative;
					previous[neighbor] = current_node.position;

					frontier.emplace(neighbor, tentative);
				}
			}
		}

		if (!previous.contains(source)) return {};

		std::vector<math::Vec2i> path;
		math::Vec2i current = source;

		while (current != target) {
			path.push_back(current);
			current = previous[current];
		}

		path.push_back(target);
		// std::reverse(path.begin(), path.end());

		return path;
	}

}

#endif // !MALAISE_DJIKSTRA_HPP
