#include "Network.h"
#include <iostream>
#include <unordered_set>
#include <queue>
#include <limits>
#include <algorithm>
#include <cmath>

Network::Network() {}

const std::unordered_map<int, double> DIAMETER_CAPACITY = {
    {500, 420},   
    {700, 820},   
    {1000, 1680}, 
    {1400, 3250}  
};

const double CAPACITY_COEFFICIENT = 1.0;

void Network::addPipe(const Pipe& pipe) {
    freePipesByDiameter[pipe.getDiametr()][pipe.getId()] = true;
}

void Network::addCS(const CS& cs) {
    graph[cs.getId()];
}

bool Network::connectCS(int csInId, int csOutId, int diameter, PipeManager& pipeManager) {
    if (diameter != 500 && diameter != 700 && diameter != 1000 && diameter != 1400) {
        std::cout << "Error: Invalid diameter! Allowed: 500, 700, 1000, 1400" << std::endl;
        return false;
    }

    if (!isValidConnection(csInId, csOutId)) {
        return false;
    }

    auto diamIt = freePipesByDiameter.find(diameter);
    if (diamIt != freePipesByDiameter.end() && !diamIt->second.empty()) {
        std::cout << "Available pipes with diameter " << diameter << ":" << std::endl;
        for (const auto& pipe : diamIt->second) {
            std::cout << "Pipe ID: " << pipe.first << std::endl;
        }

        int selectedPipeId;
        std::cout << "Select pipe ID to use: ";
        std::cin >> selectedPipeId;
        std::cin.ignore();

        if (diamIt->second.find(selectedPipeId) == diamIt->second.end()) {
            std::cout << "Error: Invalid pipe ID or pipe not available!" << std::endl;
            return false;
        }

        diamIt->second.erase(selectedPipeId);

        Pipe* pipe = pipeManager.getPipe(selectedPipeId);
        if (pipe) {
            double capacity = calculatePipeCapacity(*pipe);
            graph[csInId][csOutId] = std::make_pair(selectedPipeId, capacity);
        }
        else {
            graph[csInId][csOutId] = std::make_pair(selectedPipeId, 0.0);
        }

        std::cout << "Connected CS " << csInId << " -> CS " << csOutId
            << " using pipe " << selectedPipeId << " (diameter: " << diameter << ")" << std::endl;
        return true;
    }

    std::cout << "No free pipe with diameter " << diameter << ". Creating new pipe..." << std::endl;

    Pipe& newPipe = pipeManager.createPipeWithoutDiameter();
    newPipe.setDiametr(diameter);

    double capacity = calculatePipeCapacity(newPipe);
    graph[csInId][csOutId] = std::make_pair(newPipe.getId(), capacity);

    std::cout << "Connected CS " << csInId << " -> CS " << csOutId
        << " using NEW pipe " << newPipe.getId() << " (diameter: " << diameter
        << ", capacity: " << capacity << " m³/h)" << std::endl;
    return true;
}

double Network::calculatePipeCapacity(const Pipe& pipe) const {
    // Если труба в ремонте, производительность = 0
    if (pipe.getRepair()) {
        return 0.0;
    }

    auto it = DIAMETER_CAPACITY.find(pipe.getDiametr());
    if (it == DIAMETER_CAPACITY.end()) {
        return 0.0;
    }

    double baseCapacity = it->second;

    // Упрощенная формула: sqrt(d^5/l) * коэффициент
    double diameterM = pipe.getDiametr() / 1000.0; 
    double lengthKm = pipe.getLenght();

    if (lengthKm <= 0) {
        return baseCapacity;
    }

    double calculated = baseCapacity * sqrt(pow(diameterM, 5) / (lengthKm * 1000));
    calculated *= CAPACITY_COEFFICIENT;

    return std::max(calculated, 0.0);
}

bool Network::isValidConnection(int csInId, int csOutId) const {
    if (csInId == csOutId) {
        std::cout << "Error: Cannot connect CS to itself!" << std::endl;
        return false;
    }

    auto csInIt = graph.find(csInId);
    if (csInIt != graph.end() && csInIt->second.find(csOutId) != csInIt->second.end()) {
        std::cout << "Error: Connection already exists!" << std::endl;
        return false;
    }

    auto csOutIt = graph.find(csOutId);
    if (csOutIt != graph.end() && csOutIt->second.find(csInId) != csOutIt->second.end()) {
        std::cout << "Error: Reverse connection exists!" << std::endl;
        return false;
    }

    return true;
}

void Network::displayNetwork() const {
    std::cout << "\n=== NETWORK ===" << std::endl;

    bool hasConnections = false;
    for (const auto& csIn : graph) {
        if (!csIn.second.empty()) {
            hasConnections = true;
            std::cout << "CS " << csIn.first << " -> ";
            for (const auto& csOut : csIn.second) {
                std::cout << "CS " << csOut.first
                    << "(pipe:" << csOut.second.first
                    << ", cap:" << csOut.second.second << " m³/h) ";
            }
            std::cout << std::endl;
        }
    }

    if (!hasConnections) {
        std::cout << "No connections in network" << std::endl;
    }
}

void Network::printNetworkForFlow() const {
    std::cout << "\n=== NETWORK FOR FLOW CALCULATION ===" << std::endl;

    for (const auto& csIn : graph) {
        for (const auto& csOut : csIn.second) {
            std::cout << "CS " << csIn.first << " -> CS " << csOut.first
                << ": capacity = " << csOut.second.second << " m³/h"
                << " (pipe " << csOut.second.first << ")" << std::endl;
        }
    }
}

std::vector<int> Network::getConnectedCSIds() const {
    std::vector<int> csIds;
    for (const auto& entry : graph) {
        csIds.push_back(entry.first);
    }
    return csIds;
}

bool Network::bfsForMaxFlow(int source, int sink,
    std::unordered_map<int, int>& parent,
    const std::unordered_map<int, std::unordered_map<int, double>>& capacity) const {
    std::unordered_map<int, bool> visited;
    std::queue<int> q;

    q.push(source);
    visited[source] = true;
    parent[source] = -1;

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        // Проверяем исходящие связи
        auto it = capacity.find(u);
        if (it != capacity.end()) {
            for (const auto& neighbor : it->second) {
                int v = neighbor.first;
                double cap = neighbor.second;

                if (!visited[v] && cap > 0.0001) { // Небольшой порог, но больше
                    q.push(v);
                    parent[v] = u;
                    visited[v] = true;

                    if (v == sink) {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

double Network::findMaxFlow(int sourceId, int sinkId, PipeManager& pipeManager) const {
    if (sourceId == sinkId) {
        std::cout << "Error: Source and sink cannot be the same!" << std::endl;
        return 0.0;
    }

    if (graph.find(sourceId) == graph.end() || graph.find(sinkId) == graph.end()) {
        std::cout << "Error: Source or sink CS not found in network!" << std::endl;
        return 0.0;
    }

    std::cout << "\n=== CALCULATING MAXIMUM FLOW ===" << std::endl;
    std::cout << "Source: CS " << sourceId << std::endl;
    std::cout << "Sink: CS " << sinkId << std::endl;

    // Создаем матрицу пропускных способностей
    std::unordered_map<int, std::unordered_map<int, double>> capacity;

    // Инициализируем для всех узлов, которые есть в графе
    for (const auto& csIn : graph) {
        for (const auto& csOut : csIn.second) {
            capacity[csIn.first][csOut.first] = csOut.second.second;
            // Инициализируем обратное ребро
            capacity[csOut.first][csIn.first] = 0.0;
        }
    }

    // Добавляем узлы, которые могут быть только получателями
    for (const auto& csIn : graph) {
        // Убедимся, что узел существует в capacity
        if (capacity.find(csIn.first) == capacity.end()) {
            capacity[csIn.first] = {};
        }
    }

    std::cout << "\nNetwork capacities:" << std::endl;
    for (const auto& node : capacity) {
        for (const auto& neighbor : node.second) {
            if (neighbor.second > 0) {
                std::cout << "  CS " << node.first << " -> CS " << neighbor.first
                    << ": " << neighbor.second << " m³/h" << std::endl;
            }
        }
    }

    double maxFlow = 0.0;
    int iteration = 0;
    const int MAX_ITERATIONS = 1000; // Защита от бесконечного цикла

    while (iteration < MAX_ITERATIONS) {
        std::unordered_map<int, int> parent;

        if (!bfsForMaxFlow(sourceId, sinkId, parent, capacity)) {
            break; // Увеличивающий путь не найден
        }

        // Находим минимальную пропускную способность на пути
        double pathFlow = std::numeric_limits<double>::max();

        for (int v = sinkId; v != sourceId; v = parent[v]) {
            int u = parent[v];
            pathFlow = std::min(pathFlow, capacity[u][v]);
        }

        // Обновляем остаточные пропускные способности
        for (int v = sinkId; v != sourceId; v = parent[v]) {
            int u = parent[v];
            capacity[u][v] -= pathFlow;
            capacity[v][u] += pathFlow;
        }

        maxFlow += pathFlow;

        std::cout << "Iteration " << (iteration + 1)
            << ": found augmenting path with flow = " << pathFlow
            << ", total flow = " << maxFlow << " m³/h" << std::endl;

        iteration++;

        // Если поток очень маленький, выходим
        if (pathFlow < 0.0001) {
            break;
        }
    }

    if (iteration >= MAX_ITERATIONS) {
        std::cout << "WARNING: Maximum iterations reached!" << std::endl;
    }

    std::cout << "\n=== MAXIMUM FLOW RESULT ===" << std::endl;
    std::cout << "Source: CS " << sourceId << std::endl;
    std::cout << "Sink: CS " << sinkId << std::endl;
    std::cout << "Maximum flow: " << maxFlow << " m³/h" << std::endl;
    std::cout << "Iterations: " << iteration << std::endl;

    // Выводим остаточные пропускные способности
    std::cout << "\nResidual capacities:" << std::endl;
    for (const auto& node : capacity) {
        for (const auto& neighbor : node.second) {
            if (capacity[node.first][neighbor.first] > 0) {
                std::cout << "  CS " << node.first << " -> CS " << neighbor.first
                    << ": " << capacity[node.first][neighbor.first] << " m³/h" << std::endl;
            }
        }
    }

    return maxFlow;
}

double Network::heuristicEstimate(int current, int goal) const {
    return 0.0;
}

std::vector<int> Network::aStarSearch(int start, int goal,
    const std::unordered_map<int, std::unordered_map<int, double>>& weight) const {
    struct Node {
        int id;
        double fScore;
        double gScore;

        bool operator>(const Node& other) const {
            return fScore > other.fScore;
        }
    };

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;
    std::unordered_map<int, double> gScore;
    std::unordered_map<int, double> fScore;
    std::unordered_map<int, int> cameFrom;

    // Инициализируем все узлы в графе
    for (const auto& entry : graph) {
        gScore[entry.first] = std::numeric_limits<double>::max();
        fScore[entry.first] = std::numeric_limits<double>::max();
    }

    gScore[start] = 0.0;
    fScore[start] = heuristicEstimate(start, goal);

    openSet.push({ start, fScore[start], gScore[start] });

    while (!openSet.empty()) {
        Node current = openSet.top();
        openSet.pop();

        if (current.id == goal) {
            // Восстанавливаем путь
            std::vector<int> path;
            int node = goal;
            while (node != start) {
                path.push_back(node);
                node = cameFrom[node];
            }
            path.push_back(start);
            std::reverse(path.begin(), path.end());
            return path;
        }

        // Проверяем всех соседей текущего узла
        auto it = weight.find(current.id);
        if (it != weight.end()) {
            for (const auto& neighbor : it->second) {
                int neighborId = neighbor.first;
                double edgeWeight = neighbor.second;

                // Пропускаем недоступные ребра (трубы в ремонте)
                if (edgeWeight >= std::numeric_limits<double>::max() / 2) {
                    continue;
                }

                double tentativeGScore = gScore[current.id] + edgeWeight;

                if (tentativeGScore < gScore[neighborId]) {
                    cameFrom[neighborId] = current.id;
                    gScore[neighborId] = tentativeGScore;
                    fScore[neighborId] = tentativeGScore + heuristicEstimate(neighborId, goal);
                    openSet.push({ neighborId, fScore[neighborId], gScore[neighborId] });
                }
            }
        }
    }

    // Путь не найден
    return {};
}

std::vector<int> Network::findShortestPath(int startId, int endId, PipeManager& pipeManager, bool useHeuristic) const {
    if (startId == endId) {
        std::cout << "Error: Start and end cannot be the same!" << std::endl;
        return {};
    }

    if (graph.find(startId) == graph.end() || graph.find(endId) == graph.end()) {
        std::cout << "Error: Start or end CS not found in network!" << std::endl;
        return {};
    }

    // Создаем матрицу весов для ДВУНАПРАВЛЕННОГО графа
    std::unordered_map<int, std::unordered_map<int, double>> weight;

    // Заполняем веса для существующих соединений
    for (const auto& csIn : graph) {
        for (const auto& csOut : csIn.second) {
            int pipeId = csOut.second.first;
            Pipe* pipe = pipeManager.getPipe(pipeId);

            if (pipe) {
                double edgeWeight;
                if (pipe->getRepair()) {
                    // Если труба в ремонте - бесконечный вес
                    edgeWeight = std::numeric_limits<double>::max();
                }
                else {
                    // Вес = длина трубы
                    edgeWeight = pipe->getLenght();
                }

                // Прямое направление
                weight[csIn.first][csOut.first] = edgeWeight;
                // Обратное направление (газ может течь в обе стороны)
                weight[csOut.first][csIn.first] = edgeWeight;
            }
            else {
                // Если труба не найдена, устанавливаем бесконечный вес
                weight[csIn.first][csOut.first] = std::numeric_limits<double>::max();
                weight[csOut.first][csIn.first] = std::numeric_limits<double>::max();
            }
        }
    }

    std::cout << "\n=== SEARCHING FOR PATH ===" << std::endl;
    std::cout << "Start: CS " << startId << std::endl;
    std::cout << "End: CS " << endId << std::endl;
    std::cout << "Network connections:" << std::endl;

    // Выводим информацию о соединениях для отладки
    for (const auto& csIn : graph) {
        for (const auto& csOut : csIn.second) {
            std::cout << "  CS " << csIn.first << " <-> CS " << csOut.first;
            auto it = weight.find(csIn.first);
            if (it != weight.end()) {
                auto neighborIt = it->second.find(csOut.first);
                if (neighborIt != it->second.end()) {
                    if (neighborIt->second >= std::numeric_limits<double>::max() / 2) {
                        std::cout << " [IN REPAIR/UNREACHABLE]";
                    }
                    else {
                        std::cout << " [distance: " << neighborIt->second << " km]";
                    }
                }
            }
            std::cout << std::endl;
        }
    }

    // Выполняем поиск A*
    std::vector<int> path = aStarSearch(startId, endId, weight);

    std::cout << "\n=== SHORTEST PATH RESULT ===" << std::endl;

    if (path.empty()) {
        std::cout << "No path found between CS " << startId << " and CS " << endId << "!" << std::endl;
        std::cout << "Possible reasons:" << std::endl;
        std::cout << "1. No direct or indirect connection exists" << std::endl;
        std::cout << "2. All connecting pipes are in repair" << std::endl;
        std::cout << "3. Start or end CS is isolated" << std::endl;
        return {};
    }

    std::cout << "Path found: ";
    double totalWeight = 0.0;
    for (size_t i = 0; i < path.size(); i++) {
        std::cout << "CS " << path[i];
        if (i < path.size() - 1) {
            std::cout << " -> ";

            // Вычисляем вес ребра
            auto it = weight.find(path[i]);
            if (it != weight.end()) {
                auto neighborIt = it->second.find(path[i + 1]);
                if (neighborIt != it->second.end()) {
                    if (neighborIt->second < std::numeric_limits<double>::max() / 2) {
                        totalWeight += neighborIt->second;
                    }
                }
            }
        }
    }
    std::cout << std::endl;
    std::cout << "Total distance: " << totalWeight << " km" << std::endl;

    // Дополнительная информация о пути
    std::cout << "\nPath details:" << std::endl;
    for (size_t i = 0; i < path.size() - 1; i++) {
        int cs1 = path[i];
        int cs2 = path[i + 1];

        // Ищем трубу, соединяющую эти КС
        int pipeId = -1;
        double capacity = 0.0;

        auto it1 = graph.find(cs1);
        if (it1 != graph.end()) {
            auto it2 = it1->second.find(cs2);
            if (it2 != it1->second.end()) {
                pipeId = it2->second.first;
                capacity = it2->second.second;
            }
        }

        // Если не найдено в прямом направлении, ищем в обратном
        if (pipeId == -1) {
            auto it1_rev = graph.find(cs2);
            if (it1_rev != graph.end()) {
                auto it2_rev = it1_rev->second.find(cs1);
                if (it2_rev != it1_rev->second.end()) {
                    pipeId = it2_rev->second.first;
                    capacity = it2_rev->second.second;
                }
            }
        }

        if (pipeId != -1) {
            Pipe* pipe = pipeManager.getPipe(pipeId);
            if (pipe) {
                std::cout << "  CS " << cs1 << " -> CS " << cs2
                    << ": Pipe " << pipeId
                    << " (length: " << pipe->getLenght() << " km"
                    << ", diameter: " << pipe->getDiametr() << " mm"
                    << ", capacity: " << capacity << " m³/h"
                    << ", status: " << (pipe->getRepair() ? "IN REPAIR" : "OPERATIONAL")
                    << ")" << std::endl;
            }
        }
    }

    return path;
}
std::unordered_map<int, int> Network::topologicalSort() const {
    std::unordered_map<int, int> result;
    std::unordered_map<int, bool> visited;
    std::unordered_map<int, bool> recStack;
    std::vector<int> order;
    bool hasCycle = false;

    // Проверяем наличие циклов
    for (const auto& vertex : graph) {
        int v = vertex.first;
        if (!visited[v] && hasCycleUtil(v, visited, recStack)) {
            std::cout << "Error: Graph has cycles, cannot perform topological sort!" << std::endl;
            return result;
        }
    }

    // Сбрасываем посещенные узлы
    visited.clear();
    int index = 0;

    // Выполняем топологическую сортировку
    for (const auto& vertex : graph) {
        int v = vertex.first;
        if (!visited[v]) {
            topologicalSortUtil(v, visited, result, index);
        }
    }

    // Разворачиваем результат
    std::unordered_map<int, int> reversedResult;
    for (int i = 0; i < index; i++) {
        reversedResult[i] = result[index - 1 - i];
    }

    return reversedResult;
}

void Network::topologicalSortUtil(int v, std::unordered_map<int, bool>& visited,
    std::unordered_map<int, int>& result, int& index) const {
    visited[v] = true;

    auto it = graph.find(v);
    if (it != graph.end()) {
        for (const auto& adjacent : it->second) {
            int adjacentVertex = adjacent.first;
            if (!visited[adjacentVertex]) {
                topologicalSortUtil(adjacentVertex, visited, result, index);
            }
        }
    }

    result[index++] = v;
}

bool Network::hasCycle() const {
    std::unordered_map<int, bool> visited;
    std::unordered_map<int, bool> recStack;

    for (const auto& vertex : graph) {
        int v = vertex.first;
        if (!visited[v] && hasCycleUtil(v, visited, recStack)) {
            return true;
        }
    }
    return false;
}

bool Network::hasCycleUtil(int v, std::unordered_map<int, bool>& visited,
    std::unordered_map<int, bool>& recStack) const {
    if (!visited[v]) {
        visited[v] = true;
        recStack[v] = true;

        auto it = graph.find(v);
        if (it != graph.end()) {
            for (const auto& adjacent : it->second) {
                int adjacentVertex = adjacent.first;
                if (!visited[adjacentVertex] && hasCycleUtil(adjacentVertex, visited, recStack)) {
                    return true;
                }
                else if (recStack[adjacentVertex]) {
                    return true;
                }
            }
        }
    }
    recStack[v] = false;
    return false;
}

bool Network::hasCSConnections(int csId) const {
    // Проверяем исходящие связи
    auto csIt = graph.find(csId);
    if (csIt != graph.end() && !csIt->second.empty()) {
        return true;
    }

    // Проверяем входящие связи
    for (const auto& csIn : graph) {
        if (csIn.second.find(csId) != csIn.second.end()) {
            return true;
        }
    }

    return false;
}

bool Network::hasPipeConnections(int pipeId) const {
    for (const auto& csIn : graph) {
        for (const auto& csOut : csIn.second) {
            if (csOut.second.first == pipeId) {
                return true;
            }
        }
    }
    return false;
}

bool Network::isEmpty() const {
    if (graph.empty()) {
        return true;
    }

    for (const auto& vertex : graph) {
        if (!vertex.second.empty()) {
            return false;
        }
    }
    return true;
}

bool Network::removeCS(int csId, PipeManager& pipeManager) {
    std::unordered_set<int> pipesToFree;

    // Находим все трубы, связанные с этой КС (исходящие связи)
    auto outgoingIt = graph.find(csId);
    if (outgoingIt != graph.end()) {
        for (const auto& connection : outgoingIt->second) {
            pipesToFree.insert(connection.second.first);
        }
    }

    // Удаляем входящие связи и собираем их трубы
    for (auto& csIn : graph) {
        auto it = csIn.second.find(csId);
        if (it != csIn.second.end()) {
            pipesToFree.insert(it->second.first);
            csIn.second.erase(it);
        }
    }

    // Возвращаем трубы в список свободных
    for (int pipeId : pipesToFree) {
        Pipe* pipe = pipeManager.getPipe(pipeId);
        if (pipe) {
            freePipesByDiameter[pipe->getDiametr()][pipeId] = true;
        }
    }

    // Удаляем саму КС из графа
    graph.erase(csId);

    return true;
}

bool Network::removePipe(int pipeId, PipeManager& pipeManager, bool returnToFree) {
    bool found = false;

    for (auto& csIn : graph) {
        for (auto it = csIn.second.begin(); it != csIn.second.end(); ) {
            if (it->second.first == pipeId) {
                if (returnToFree) {
                    Pipe* pipe = pipeManager.getPipe(pipeId);
                    if (pipe) {
                        freePipesByDiameter[pipe->getDiametr()][pipeId] = true;
                    }
                }

                it = csIn.second.erase(it);
                found = true;
            }
            else {
                ++it;
            }
        }
    }

    return found;
}