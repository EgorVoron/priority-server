#include <bits/stdc++.h>
#include "json/json.hpp"
#include "utils.h"

using namespace std;

using json = nlohmann::json;

struct Node {
    uint32_t id;
    uint32_t uid, priority;
    json payload;

    Node(uint32_t id, uint32_t uid, uint32_t priority, json &payload) : id(id), uid(uid), priority(priority),
                                                                           payload(payload) {}

    json toJson() {
        json j;
        j["id"] = id;
        j["uid"] = uid;
        j["priority"] = priority;
        j["payload"] = payload;
        return j;
    }

    vector<uint8_t> toBson() {
        auto b = json::to_bson(toJson());
        return b;
    }

    string toString() {
        return toJson().dump();
    }
};

Node nodeFromJson(json j) {
    Node node = Node(j["id"], j["uid"], j["priority"], j["payload"]);
    return node;
}

uint32_t lastIdx(vector<Node> &array) {
    assert(!array.empty());
    return array.size() - 1;
}

uint32_t parent(uint32_t i) { return i / 2; }
uint32_t leftChild(uint32_t i) { return i * 2; }
uint32_t rightChild(uint32_t i) { return i * 2 + 1; }

class PriorityQueue {
private:
    vector<Node> array;
    map<uint32_t, uint32_t> id2idx;
    map<uint32_t, set<uint32_t>> userNodesIds;
    map<uint32_t, uint32_t> nodeUser;

    string filePath;

    void fullSwap(Node *a, Node *b);
    void siftUp(uint32_t i);
    void siftDown(uint32_t i);
    void clearContainers();

public:
    bool isEmpty();
    PriorityQueue(string path);
    ~PriorityQueue();
    uint32_t insert(uint32_t uid, uint32_t priority, json payload, uint32_t randll);
    void extractMax();
    void erase(uint32_t id);
    Node get(uint32_t id);
    Node getMax();
    set<uint32_t> getUserNodes(uint32_t uid);
    vector<uint32_t> changeUserNodes(uint32_t uid, uint32_t priority);
    bool exists(uint32_t id);
    bool userExists(uint32_t uid);
    void print();
};

PriorityQueue::PriorityQueue(string path) : filePath(std::move(path)) {
    if (correctFile(filePath)) {
        try {
            ifstream infile(filePath);
//            uint8_t tmp;
            vector<uint8_t> vectorBson;
            std::string contents((std::istreambuf_iterator<char>(infile)),
                                 std::istreambuf_iterator<char>());
            json listJson = json::from_bson(contents)["data"];
            // tested
//            for (const auto& nodeJson : listJson) {
//                Node node = nodeFromJson(nodeJson);
//
//                array.push_back(node);
//                id2idx[node.id] = (array.size() - 1);
//                siftUp(array.size() - 1);
//
//                userNodesIds[node.uid].insert(node.id);
//                nodeUser[node.id] = node.uid;
//            }
            for (const auto& nodeJson : listJson) {
                Node node = nodeFromJson(nodeJson);

                array.push_back(node);
                id2idx[node.id] = lastIdx(array);
//                siftUp(array.size() - 1);

                userNodesIds[node.uid].insert(node.id);
                nodeUser[node.id] = node.uid;
            }
        } catch (exception &e) {
            cout << "Cannot serialize queue: " << e.what() << '\n';
        }
        catch (...) {
            cout << "Cannot serialize queue: unknown error\n";
        }
    }
    print();
}

void PriorityQueue::clearContainers() {
    id2idx.clear();
    userNodesIds.clear();
    nodeUser.clear();
    array.clear();
}

PriorityQueue::~PriorityQueue() {
    if (!isEmpty()) {
        json savedQueueJsonList;
        while (!isEmpty()) {
            Node max = getMax();
            savedQueueJsonList.push_back(max.toJson());
            extractMax();
        }
        json savedQueueJsonDict;
        savedQueueJsonDict["data"] = savedQueueJsonList;
        cout << savedQueueJsonDict;
        ofstream outfile(filePath);
        vector<uint8_t> savedQueueBson = json::to_bson(savedQueueJsonDict);
        for (uint8_t n : savedQueueBson) {
            outfile << n;
        }
        outfile.close();
        savedQueueJsonDict.clear();
        savedQueueJsonList.clear();
        savedQueueBson.clear();
    }
    else {
        std::ofstream ofs;
        ofs.open(filePath, std::ofstream::out | std::ofstream::trunc);
        ofs.close();
    }
    clearContainers();
}

void PriorityQueue::fullSwap(Node *a, Node *b) {
    swap(*a, *b);
    swap(id2idx[a->id], id2idx[b->id]);
}

void PriorityQueue::siftUp(uint32_t i) {
    while ((i > 0) && (array[i].priority > array[parent(i)].priority)) {
        fullSwap(&array[i], &array[parent(i)]);
        i /= 2;
    }
}

void PriorityQueue::siftDown(uint32_t i) {
    while (2 * i < array.size()) {
        uint32_t j = -1;
        if (array[leftChild(i)].priority > array[i].priority) {
            j = leftChild(i);
        }
        if (rightChild(i) < array.size() && array[rightChild(i)].priority > array[i].priority) {
            if ((j == -1) || (array[leftChild(i)].priority < array[rightChild(i)].priority)) {
                j = rightChild(i);
            }
        }
        if (j == -1) {
            break;
        } else {
            fullSwap(&array[i], &array[j]);
            i = j;
        }
    }
}

bool PriorityQueue::isEmpty() {
    return array.empty();
}

uint32_t PriorityQueue::insert(uint32_t uid, uint32_t priority, json payload, uint32_t randll) {
    Node node = Node(randll, uid, priority, payload);
    array.push_back(node);
    id2idx[node.id] = lastIdx(array);
    siftUp(lastIdx(array));

    userNodesIds[uid].insert(node.id);
    nodeUser[node.id] = uid;
    return node.id;
}

void PriorityQueue::extractMax() {
    uint32_t id = array[0].id;

    fullSwap(&array[0], &array.back());
    array.pop_back();
    siftDown(0);

    uint32_t user = nodeUser[id];
    userNodesIds[user].erase(id);
    nodeUser.erase(id);
    id2idx.erase(id);
}

void PriorityQueue::erase(uint32_t id) {
    uint32_t idx = id2idx[id];
    fullSwap(&array[idx], &array.back());
    array.pop_back();


    siftUp(idx);
    siftDown(idx);

    uint32_t user = nodeUser[id];
    userNodesIds[user].erase(id);
    nodeUser.erase(id);
    id2idx.erase(id);
}

Node PriorityQueue::get(uint32_t id) {
    return array[id2idx[id]];
}

Node PriorityQueue::getMax() {
    return array[0];
}

set<uint32_t> PriorityQueue::getUserNodes(uint32_t uid) {
    return userNodesIds[uid];
}

vector<uint32_t> PriorityQueue::changeUserNodes(uint32_t uid, uint32_t newPriority) {
    vector<uint32_t> ans;
//    for (auto i : userNodesIds[uid]) cout << i << " ";
//    cout << endl;
    set<uint32_t> setForUser = userNodesIds[uid];
    for (uint32_t userNodeId : setForUser) {
        Node node = get(userNodeId);
        Node nodeFixed = Node(node.id, node.uid, newPriority, node.payload);
        erase(node.id);
        insert(nodeFixed.uid, nodeFixed.priority, nodeFixed.payload, nodeFixed.id);
        ans.push_back(userNodeId);
    }
    print();
    return ans;
}

bool PriorityQueue::exists(uint32_t id) {
    return (id2idx.find(id) != id2idx.end());
}

bool PriorityQueue::userExists(uint32_t uid) {
    return (userNodesIds.find(uid) != userNodesIds.end());
}

void PriorityQueue::print() {
    for (const auto& i : array) {
        cout << i.uid << " " << i.priority << " " << i.id << endl;
    }
    cout << "-----------------------------";
    cout << endl;
}