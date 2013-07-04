#pragma once
#include <string>
#include <memory>
#include <vector>
#include "analyzer.hpp"
#include "indexing.hpp"

namespace indexing {

struct QueryItem {
    int docId;
    double score;
};

struct Query {
    virtual ~Query() = default;
    virtual bool next(QueryItem&) = 0;
};

struct AndQuery : public Query {
    virtual bool next(QueryItem& item) {
        QueryItem u, v;
        if (left->next(u) && right->next(v)) {
            while (true) {
                if (u.docId == v.docId) {
                    item.docId = u.docId;
                    item.score = u.score * leftFactor + v.score * rightFactor;
                    return true;
                } else if (u.docId < v.docId) {
                    if (!left->next(u)) return false;
                } else {
                    if (!right->next(v)) return false;
                }
            }
        } else {
            return false;
        }
    }

    AndQuery(std::unique_ptr<Query> leftOp, std::unique_ptr<Query> rightOp, double leftWeight = 1, double rightWeight = 1)
    : leftFactor(leftWeight), rightFactor(rightWeight)
    {
    left = std::move(leftOp);
    right = std::move(rightOp);
    }

    std::unique_ptr<Query> left, right;
    double leftFactor, rightFactor;
};

struct OrQuery : public Query {

    virtual bool next(QueryItem& item) {
        if (!hasLeft) hasLeft = left->next(u);
        if (!hasRight) hasRight = right->next(v);

        if (hasLeft) {
            if (hasRight) {
                if (u.docId == v.docId) {
                    item.docId = u.docId;
                    item.score = u.score * leftFactor + v.score * rightFactor;
                    hasLeft = false;
                    hasRight = false;
                } else if (u.docId < v.docId) {
                    item = u;
                    hasLeft = false;
                } else {
                    item = v;
                    hasRight = false;
                }
            } else {
                item = u;
            }
        } else if (hasRight) {
            item = v;
        } else {
            return false;
        }
        return true;
    }

    OrQuery(std::unique_ptr<Query> leftOp, std::unique_ptr<Query> rightOp, double leftWeight = 1, double rightWeight = 1)
    : leftFactor(leftWeight), rightFactor(rightWeight)
    {
        left = std::move(leftOp);
        right = std::move(rightOp);
    }


    std::unique_ptr<Query> left, right;
    double leftFactor, rightFactor;

private:
    bool hasLeft = false, hasRight = false;
    QueryItem u, v;
};

struct TermQuery : public Query {

private:
    PostingList::iterator it, end;

public:
    TermQuery(const Index& index, Term &term) {
        if (index.find(term) != index.end())
    {
        it = index.find(term)->second.begin();
        end = index.find(term)->second.end();
        }
        else
            it = end = index.begin()->second.end();
    }

    virtual bool next(QueryItem& item) {
        if (it == end) return false;
        item.docId = it->docId;
        item.score = it->score;
        it++;
        return true;
    }
};


// Query Analyzers

struct QueryAnalyzer {
};

struct StandardQueryAnalyzer : public QueryAnalyzer
{
    std::unique_ptr<Query> TryCreateTermQuery(const std::string& term, const Index& index);
    std::unique_ptr<Query> BuildOrQueryTree(std::vector< std::unique_ptr<Query> >& queries, int start, int end);
    std::unique_ptr<Query> BuildAndQueryTree(std::vector< std::unique_ptr<Query> >& queries, int start, int end);
    std::unique_ptr<Query> MergeWithOrQuery(std::unique_ptr<Query>& left, std::unique_ptr<Query>& right);
    std::unique_ptr<Query> MergeWithAndQuery(std::unique_ptr<Query>& left, std::unique_ptr<Query>& right);
};

std::unique_ptr<Query> StandardQueryAnalyzer::BuildOrQueryTree(std::vector< std::unique_ptr<Query> >& queries, int start, int end)
{
    if(start==end)
        return std::move(queries[start]);
    int middle = ((start+end)>>1);
    std::unique_ptr<Query> left = std::move(BuildOrQueryTree(queries, start, middle));
    std::unique_ptr<Query> right = std::move(BuildOrQueryTree(queries, middle+1, end));
    return std::move(MergeWithOrQuery(left, right));
}

std::unique_ptr<Query> StandardQueryAnalyzer::MergeWithOrQuery(std::unique_ptr<Query>& left, std::unique_ptr<Query>& right)
{
    if (!left)
        return std::move(right);
    else if (!right)
        return std::move(left);
    else
        return std::move(std::unique_ptr<Query> (new OrQuery(std::move(left), std::move(right))));
}

std::unique_ptr<Query> StandardQueryAnalyzer::BuildAndQueryTree(std::vector< std::unique_ptr<Query> >& queries, int start, int end)
{
    if(start==end)
        return std::move(queries[start]);
    int middle = ((start+end)>>1);
    std::unique_ptr<Query> left = std::move(BuildAndQueryTree(queries, start, middle));
    std::unique_ptr<Query> right = std::move(BuildAndQueryTree(queries, middle+1, end));
    return std::move(MergeWithAndQuery(left, right));
}

std::unique_ptr<Query> StandardQueryAnalyzer::MergeWithAndQuery(std::unique_ptr<Query>& left, std::unique_ptr<Query>& right)
{
    if (!left || !right)
        return NULL;
    else
        return std::move(std::unique_ptr<Query> (new AndQuery(std::move(left), std::move(right))));
}

std::unique_ptr<Query> StandardQueryAnalyzer::TryCreateTermQuery(const std::string& term_string, const Index& index)
{
    int term_id = index.word_map.find_id(term_string);
    if (term_id == -1) return NULL;
    int field_id = 0;
    Term term{term_id, field_id};
    std::unique_ptr<Query> p (new TermQuery(index, term));
    return p;
}

std::unique_ptr<Query> buildQuery(const std::string& query, const Index& index) {
    StandardQueryAnalyzer analyzer;

    std::unique_ptr<TokenStream> stream(ArnetAnalyzer::tokenStream(query));
    Token token;
    std::vector<std::unique_ptr<Query>> queries;

    queries.clear();
    while (stream->next(token))
    {
        std::unique_ptr<Query> p = analyzer.TryCreateTermQuery(token.getTermText(), index);
        if (p != NULL)
            queries.push_back(std::move(p));
    }
    std::unique_ptr<Query> andQueryTree = analyzer.BuildAndQueryTree(queries, 0, queries.size()- 1);

    queries.clear();
    std::unique_ptr<TokenStream> stream2(ArnetAnalyzer::tokenStream(query));
    while (stream2->next(token))
    {
        std::unique_ptr<Query> p = analyzer.TryCreateTermQuery(token.getTermText(), index);
        if (p != NULL)
            queries.push_back(std::move(p));
    }
    std::unique_ptr<Query> orQueryTree = analyzer.BuildOrQueryTree(queries, 0, queries.size() - 1);

    if (andQueryTree == NULL)
        return std::move(orQueryTree);
    else if (orQueryTree == NULL)
        return std::move(andQueryTree);
    else
        return std::move(std::unique_ptr<Query>(new OrQuery(std::move(andQueryTree), std::move(orQueryTree), 5.0)));
}

} // namespace indexing
