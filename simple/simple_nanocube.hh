#pragma once

#include <vector>
#include <cstdint>
#include <iostream>

#include <unordered_set>

struct Node;
struct ParentChildLink;
struct ContentLink;

//-----------------------------------------------------------------------------
// Label, DimAddress and Address
//-----------------------------------------------------------------------------

using  Label      = int;
using  DimAddress = std::vector<Label>;
using  RawAddress    = std::vector<DimAddress>;

//-----------------------------------------------------------------------------
// Address
//-----------------------------------------------------------------------------

struct Address {
    Address() = default;
    Address(RawAddress data);

    Address(const Address& other) = default;
    Address& operator=(const Address& other) = default;
    Address(Address&& other) = default;
    Address& operator=(Address&& other) = default;

    Address& appendDimension();
    Address& appendLabel(Label label);

    size_t size() const;
    size_t size(int index) const;

    DimAddress& operator[](size_t index);
    const DimAddress& operator[](size_t index) const;

public:
    RawAddress data;
};

std::ostream& operator<<(std::ostream& os, const Address& addr);

//-----------------------------------------------------------------------------
// Content
//-----------------------------------------------------------------------------

struct Content {
public:
    void setOwner(Node* owner);

    Node* owner { nullptr };
};

//-----------------------------------------------------------------------------
// Object and Summary
//-----------------------------------------------------------------------------

using  Object = int;

struct Summary: public Content {
    Summary() = default;

    Summary(const Summary& other) = default;
    Summary& operator=(const Summary& other) = default;

    std::string info() const;
    void insert(const Object &obj);

public:

    std::unordered_set<Object> objects;
};

//-----------------------------------------------------------------------------
// LinkType
//-----------------------------------------------------------------------------

enum LinkType { SHARED, PROPER };

//-----------------------------------------------------------------------------
// ContentLink
//-----------------------------------------------------------------------------

struct ContentLink {

    ContentLink() = default;
    ContentLink(Content* content, LinkType link_type);

    ContentLink(const ContentLink& other) = default;
    ContentLink& operator=(const ContentLink& other) = default;


    Content*  content   { nullptr }; // might be a node or a summary
    LinkType  link_type { PROPER };
};

//-----------------------------------------------------------------------------
// ParentChildLink
//-----------------------------------------------------------------------------

struct ParentChildLink {
    ParentChildLink() = default;
    ParentChildLink(Label label, Node* child, LinkType link_type);

    ParentChildLink(const ParentChildLink& other) = default;
    ParentChildLink& operator=(const ParentChildLink& other) = default;

    Label    label { 0 };
    Node*    child { nullptr };
    LinkType link_type { PROPER };
};

//-----------------------------------------------------------------------------
// Nanocube
//-----------------------------------------------------------------------------

struct Nanocube {
public:
    Nanocube() = default;
    Nanocube(const std::vector<int> &levels);

    Nanocube(const Nanocube &nc) = delete;
    Nanocube& operator=(const Nanocube &nc) = delete;

    Nanocube(Nanocube &&nc);
    Nanocube& operator=(Nanocube &&nc);

    Summary* query(const Address &addr);
    Node*    getNode(const Address &addr);

    void insert(const Address &addr, const Object &object);

public:
    Node *root { nullptr };
    std::vector<int> levels;
    int              dimension { 0 };
};

//-----------------------------------------------------------------------------
// Node
//-----------------------------------------------------------------------------

struct Node: public Content {
public:
    enum Flag { NONE, IN_MAIN_PATH, IN_PARALLEL_PATH };

public:
    Node* shallowCopy() const;
    Content* getContent() const;
    void  setContent(Content *content, LinkType type);

    LinkType getContentType() const;
    bool     hasContent() const;

    Summary* getContentAsSummary() const;
    Node*    getContentAsNode() const;

    int      getNumChildren() const;

    // ParentChildLink* getParentChildLink(Label label) const;
    // ParentChildLink* getOrCreateParentChildLink(Label label, bool &created);
    void setParentChildLink(Label label, Node *node, LinkType link_type);
    void setParentChildLink(Label label, Node* node, LinkType link_type, bool &created);

    Node* getChild(Label label) const;
    Node* getChild(Label label, LinkType &link_type) const;

    void setProperParent(Node* parent, Label lbl);

    void setFlag(Flag flag) const;

public:
    Node* proper_parent  { nullptr }; // defined once, never changes
    Label label {-1};

    mutable Flag  flag { NONE };

    std::vector<ParentChildLink> children;
    ContentLink content_link;
};
