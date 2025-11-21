#include "Pipe.h"
#include <unordered_map>
#include <vector>
#include <string>

class PipeManager {
private:
    std::unordered_map<int, Pipe> pipes;

public:
    void addPipe(const Pipe& pipe);
    Pipe& createPipe();

    bool deletePipe(int id);
    void deletePipes(const std::vector<int>& ids);

    Pipe* getPipe(int id);

    void batchEditPipes(const std::vector<int>& ids);

    void displayAllPipes() const;
    void displayPipes(const std::vector<int>& ids) const;

    bool isEmpty() const { return pipes.empty(); }
    size_t getCount() const { return pipes.size(); }
    std::vector<int> getAllIds() const;

    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);

    const std::unordered_map<int, Pipe>& getPipes() const { return pipes; }
};
