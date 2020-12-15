#include <bits/stdc++.h>
#include "json/json.hpp"
#include "utils.h"

using namespace std;

using json = nlohmann::json;

struct Node {
    long long id;
    long long uid, priority;
    json payload;

    Node(long long id, long long uid, long long priority, json &payload) : id(id), uid(uid), priority(priority),
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

    void print() {
        cout << "id: " << id << " prio: " << priority << " uid: " << uid << " payload: " << payload << endl;
    }
};

Node nodeFromJson(json j) {
    Node node = Node(j["id"], j["uid"], j["priority"], j["payload"]);
    return node;
}

long long parent(long long i) { return i / 2; }

long long leftChild(long long i) { return i * 2; }

long long rightChild(long long i) { return i * 2 + 1; }

class PriorityQueue {
private:
    vector<Node> array;
    map<long long, long long> id2idx;

    map<long long, set<long long>> userNodesIds;
    map<long long, long long> nodeUser;

    string filePath;

    void fullSwap(Node *a, Node *b);

    void siftUp(long long i);

    void siftDown(long long i);

    void clearContainers();

public:
    bool isEmpty();

    PriorityQueue(string path = "queue.bson");

    ~PriorityQueue();

    void deserialize();

    long long insert(long long uid, long long priority, json payload, long long randll);

    void erase(long long id);

    Node get(long long int id);

    Node getMax();

    set<long long> getUserNodes(long long uid);

    vector<long long> changeUserNodes(long long uid, long long priority);

    bool exists(long long id);

    bool userExists(long long uid);

    void print();
};

PriorityQueue::PriorityQueue(string path) : filePath(std::move(path)) {
    if (correctFile(filePath)) {
//        try {
        ifstream infile(filePath);
        uint8_t tmp;
        vector<uint8_t> vectorBson;
        while (infile >> tmp) {
            vectorBson.push_back(tmp);
        }
        json listJson = json::from_bson(vectorBson)["data"];
        for (const auto& nodeJson : listJson) {
            Node node = nodeFromJson(nodeJson);

            array.push_back(node);
            id2idx[node.id] = (array.size() - 1);
            siftUp(array.size() - 1);

            // plan 56
            userNodesIds[node.uid].insert(node.id);
            nodeUser[node.id] = node.uid;
        }
//        } catch (...) {}
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
        while (!array.empty()) {
            auto max = getMax();
            savedQueueJsonList.push_back(max.toJson());
            erase(0);
        }
        json savedQueueJsonDict;
        savedQueueJsonDict["data"] = savedQueueJsonList;

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
    clearContainers();
}

void PriorityQueue::deserialize() {

}

void PriorityQueue::fullSwap(Node *a, Node *b) {
    if ((*a).priority != (*b).priority) {
        swap(*a, *b);
//        cout << "id2idx: ";
//        for (int i = 0; i < id2idx.size(); i++) {cout << i << ": " << id2idx[i] << " ";}
        swap(id2idx[a->id], id2idx[b->id]);
    }
}

void PriorityQueue::siftUp(long long i) {
    while ((i > 0) && (array[i].priority > array[parent(i)].priority)) {
        fullSwap(&array[i], &array[parent(i)]);
        i /= 2;
    }
}

void PriorityQueue::siftDown(long long i) {
    while (2 * i < array.size()) {
        long long j = -1;
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

long long PriorityQueue::insert(long long uid, long long priority, json payload, long long randll) {
    Node node = Node(randll, uid, priority, payload);
    array.push_back(node);
    id2idx[node.id] = (array.size() - 1);
    siftUp(array.size() - 1);
//    cout << "sifted up";

    // plan 56
    userNodesIds[uid].insert(node.id);
    nodeUser[node.id] = uid;
    return node.id;
}

// адекватно
void PriorityQueue::erase(long long id) {
    long long idx = id2idx[id];
    fullSwap(&array[idx], &array[array.size() - 1]);
    array.pop_back();

//    fullSwap(&array[idx], &array[0]);
//    siftDown(0);
    siftUp(idx);
    siftDown(idx);

    // plan 56
    long long user = nodeUser[id];
    userNodesIds[user].erase(id);
    nodeUser.erase(id);
    id2idx.erase(id);
}

Node PriorityQueue::get(long long id) {
    return array[id2idx[id]];
}

Node PriorityQueue::getMax() {
    return array[0];
}

set<long long> PriorityQueue::getUserNodes(long long uid) {
    return userNodesIds[uid];
}

vector<long long> PriorityQueue::changeUserNodes(long long uid, long long newPriority) {
    vector<long long> ans;
    for (auto userNodeId : userNodesIds[uid]) {
        array[id2idx[userNodeId]].priority = newPriority;
        ans.push_back(userNodeId);
    }
    return ans;
}

bool PriorityQueue::exists(long long id) {
    return (id2idx.find(id) != id2idx.end());
}

bool PriorityQueue::userExists(long long uid) {
    return (userNodesIds.find(uid) != userNodesIds.end());
}

void PriorityQueue::print() {
    for (const auto& i : array) {
        cout << i.uid << " " << i.priority << " " << i.id << endl;
    }
    cout << "-----------------------------";
    cout << endl;
}
