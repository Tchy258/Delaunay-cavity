#ifndef MESH_GENERATOR_DATA
#define MESH_GENERATOR_DATA
#include <misc/mesh_stat.hpp>
#include <misc/time_stat.hpp>
#include <misc/memory_stat.hpp>
#include <unordered_map>
#include <type_traits>

struct MeshGeneratorData {
    private:
        template<typename KeyType, typename ValueType, typename Map>
        void storeTotal(Map& map, KeyType totalKey) {
            ValueType total = static_cast<ValueType>(0);
            for (const auto& [key, value] : map) {
                if constexpr (std::is_same_v<KeyType, TimeStat>) {
                    if (key == T_TRAVERSAL_AND_REPAIR) {
                        continue;
                    }
                }
                total += value;
            }
            map[totalKey] = total;
        }
    public:
        std::unordered_map<MeshStat, int> meshStats;
        std::unordered_map<TimeStat, double> timeStats;
        std::unordered_map<MemoryStat, unsigned long long> memoryStats;

        MeshGeneratorData() {
            meshStats[N_POLYGONS] = 0;
            meshStats[N_VERTICES] = 0;
            meshStats[N_EDGES] = 0;

            timeStats[T_TRIANGULATION_GENERATION] = 0.0;
            memoryStats[M_EDGES_INPUT] = 0;
            memoryStats[M_EDGES_OUTPUT] = 0;
            memoryStats[M_VERTICES_INPUT] = 0;
            memoryStats[M_VERTICES_OUTPUT] = 0;
        }

        void computeTotalMemoryUsage() {
            storeTotal<MemoryStat,unsigned long long>(memoryStats, M_TOTAL);
        }

        void computeTotalTime() {
            storeTotal<TimeStat, double>(timeStats, T_TOTAL);
        }
};

#endif //MESH_GENERATOR_DATA