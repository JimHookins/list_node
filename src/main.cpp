#include <bits/stdc++.h>

// A node in the linked list (as it was in the task description)
struct ListNode {
    ListNode* prev = nullptr;
    ListNode* next = nullptr;
    ListNode* rand = nullptr;
    std::string data;
};

// Build a list from the text file
// return pointer to the head
// The format of the text file is:
// [data];[rand_index]
// [data];[rand_index]
// ...
ListNode* buildListFromText(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) {
        throw std::runtime_error("Failed to open the file " + filename);
    }

    std::vector<ListNode*> nodes;
    std::vector<int> randIdx;

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty())
            continue;

        auto pos = line.rfind(';');
        if (pos == std::string::npos) {
            throw std::runtime_error("Invalid line format (no ';'): " + line);
        }

        std::string data = line.substr(0, pos);
        std::string idxStr = line.substr(pos + 1);

        int idx = std::stoi(idxStr);
        if (idx < -1) {
            throw std::runtime_error("rand_index must be -1 or a valid node index");
        }

        ListNode* node = new ListNode;
        node->data = std::move(data);

        if (!nodes.empty()) {
            node->prev = nodes.back();
            nodes.back()->next = node;
        }

        nodes.push_back(node);
        randIdx.push_back(idx);
    }

    const size_t n = nodes.size();
    for (size_t i = 0; i < n; ++i) {
        const int idx = randIdx[i];
        if (idx == -1) {
            nodes[i]->rand = nullptr;
            continue;
        }

        if (idx < 0 || idx >= static_cast<int>(n)) {
            throw std::runtime_error("rand_index out of range");
        }
        nodes[i]->rand = nodes[static_cast<size_t>(idx)];
    }

    return nodes.empty() ? nullptr : nodes.front();
}

// Serialize the list to a binary file
void serializeList(ListNode* head, const std::string& filename) {
    std::vector<ListNode*> nodes;
    for (ListNode* node = head; node != nullptr; node = node->next) {
        nodes.push_back(node);
    }

    // make a map of the nodes to their indices
    const uint64_t count = nodes.size();
    std::unordered_map<ListNode*, uint32_t> indexOf;
    indexOf.reserve(static_cast<size_t>(count) * 2);
    for (uint32_t i = 0; i < static_cast<uint32_t>(count); ++i) {
        indexOf[nodes[static_cast<size_t>(i)]] = i;
    }

    std::ofstream out(filename, std::ios::binary);
    if (!out) {
        throw std::runtime_error("Failed to open outlet.out");
    }

    // There was no format specified, so I decided to do it this way
    // Binary file format:
    // [count]
    // then for each node in order:
    // [len][data bytes][rand_index]
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (auto* node : nodes) {
        const uint64_t len = node->data.size();
        out.write(reinterpret_cast<const char*>(&len), sizeof(len));
        if (len > 0) {
            out.write(node->data.data(), len);
        }

        int32_t r = -1;
        if (node->rand != nullptr) {
            auto it = indexOf.find(node->rand);
            if (it == indexOf.end()) {
                throw std::runtime_error("rand pointer points outside the list");
            }
            r = static_cast<int32_t>(it->second);
        }
        out.write(reinterpret_cast<const char*>(&r), sizeof(r));
    }
}

// Free the resources allocated for the list
void freeList(ListNode* head) {
    while (head) {
        ListNode* next = head->next;
        delete head;
        head = next;
    }
}

// Program accepts the input file as an argument
int main(int argc, char* argv[]) {
    try {
        const std::string inputFile = (argc > 1) ? argv[1] : "inlet.in";

        ListNode* head = buildListFromText(inputFile);
        serializeList(head, "outlet.out");
        freeList(head);
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }
}

