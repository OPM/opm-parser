/*
  Copyright 2013 Statoil ASA.

  This file is part of the Open Porous Media project (OPM).

  OPM is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  OPM is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with OPM.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <opm/parser/eclipse/EclipseState/Schedule/GroupTree.hpp>

#define BOOST_TEST_MODULE GroupTreeTests

#include <boost/test/unit_test.hpp>

#include <stdexcept>
#include <iostream>



using namespace Opm;

BOOST_AUTO_TEST_CASE(CreateGroupTree_DefaultConstructor_HasFieldNode) {
    GroupTree tree;
    BOOST_CHECK(tree.getNode("FIELD"));
}

BOOST_AUTO_TEST_CASE(GetNode_NonExistingNode_ReturnsNull) {
    GroupTree tree;
    BOOST_CHECK_EQUAL(GroupTreeNodePtr(), tree.getNode("Non-existing"));
}

BOOST_AUTO_TEST_CASE(GetNodeAndParent_AllOK) {
    ParserLogPtr parserLog(new ParserLog);
    GroupTree tree;
    tree.updateTree("GRANDPARENT", "FIELD", parserLog, "", -1);
    tree.updateTree("PARENT", "GRANDPARENT", parserLog, "", -1);
    tree.updateTree("GRANDCHILD", "PARENT", parserLog, "", -1);

    GroupTreeNodePtr grandchild = tree.getNode("GRANDCHILD");
    BOOST_CHECK(grandchild);
    GroupTreeNodePtr parent = tree.getParent("GRANDCHILD");
    BOOST_CHECK_EQUAL("PARENT", parent->name());
    BOOST_CHECK(parent->hasChildGroup("GRANDCHILD"));
}

BOOST_AUTO_TEST_CASE(UpdateTree_ParentNotSpecified_AddedUnderField) {
    ParserLogPtr parserLog(new ParserLog);
    GroupTree tree;
    tree.updateTree("CHILD_OF_FIELD", parserLog, "", -1);
    BOOST_CHECK(tree.getNode("CHILD_OF_FIELD"));
    GroupTreeNodePtr rootNode = tree.getNode("FIELD");
    BOOST_CHECK(rootNode->hasChildGroup("CHILD_OF_FIELD"));
}

BOOST_AUTO_TEST_CASE(UpdateTree_ParentIsField_AddedUnderField) {
    ParserLogPtr parserLog(new ParserLog);
    GroupTree tree;
    tree.updateTree("CHILD_OF_FIELD", "FIELD", parserLog, "", -1);
    BOOST_CHECK(tree.getNode("CHILD_OF_FIELD"));
    GroupTreeNodePtr rootNode = tree.getNode("FIELD");
    BOOST_CHECK(rootNode->hasChildGroup("CHILD_OF_FIELD"));
}

BOOST_AUTO_TEST_CASE(UpdateTree_ParentNotAdded_ChildAndParentAdded) {
    ParserLogPtr parserLog(new ParserLog);
    GroupTree tree;
    tree.updateTree("CHILD", "NEWPARENT", parserLog, "", -1);
    BOOST_CHECK(tree.getNode("CHILD"));
    GroupTreeNodePtr rootNode = tree.getNode("FIELD");
    BOOST_CHECK(rootNode->hasChildGroup("NEWPARENT"));
    GroupTreeNodePtr newParent = tree.getNode("NEWPARENT");
    BOOST_CHECK(newParent->hasChildGroup("CHILD"));
}

BOOST_AUTO_TEST_CASE(UpdateTree_AddFieldNode_Throws) {
    ParserLogPtr parserLog(new ParserLog);
    GroupTree tree;
    BOOST_CHECK_EQUAL(tree.updateTree("FIELD", "NEWPARENT", parserLog, "", -1), false);
    BOOST_CHECK_EQUAL(tree.updateTree("FIELD", parserLog, "", -1), false);
}

BOOST_AUTO_TEST_CASE(UpdateTree_ChildExists_ChildMoved) {
    ParserLogPtr parserLog(new ParserLog);
    GroupTree tree;
    tree.updateTree("OLDPARENT", "FIELD", parserLog, "", -1);
    tree.updateTree("NEWPARENT", "FIELD", parserLog, "", -1);
    tree.updateTree("THECHILD", "OLDPARENT", parserLog, "", -1);
    tree.updateTree("GRANDCHILD1", "THECHILD", parserLog, "", -1);
    tree.updateTree("GRANDCHILD2", "THECHILD", parserLog, "", -1);

    GroupTreeNodePtr oldParent = tree.getNode("OLDPARENT");
    BOOST_CHECK(oldParent->hasChildGroup("THECHILD"));
    GroupTreeNodePtr theChild = oldParent->getChildGroup("THECHILD");
    BOOST_CHECK(theChild->hasChildGroup("GRANDCHILD1"));
    
    GroupTreeNodePtr newParent = tree.getNode("NEWPARENT");
    BOOST_CHECK(!newParent->hasChildGroup("THECHILD"));

    tree.updateTree("THECHILD", "NEWPARENT", parserLog, "", -1);
    
    BOOST_CHECK(!oldParent->hasChildGroup("THECHILD"));
    
    BOOST_CHECK(newParent->hasChildGroup("THECHILD"));
    theChild = newParent->getChildGroup("THECHILD");
    BOOST_CHECK(theChild->hasChildGroup("GRANDCHILD1"));
}

BOOST_AUTO_TEST_CASE(DeepCopy_TreeWithChildren_ObjectsDifferContentMatch) {
    ParserLogPtr parserLog(new ParserLog);
    GroupTreePtr tree(new GroupTree());
    tree->updateTree("L1CHILD1", "FIELD", parserLog, "", -1);
    tree->updateTree("L1CHILD2", "FIELD", parserLog, "", -1);
    tree->updateTree("L2CHILD1", "L1CHILD1", parserLog, "", -1);
    tree->updateTree("L2CHILD2", "L1CHILD1", parserLog, "", -1);
    tree->updateTree("L3CHILD1", "L2CHILD1", parserLog, "", -1);

    GroupTreePtr copiedTree = tree->deepCopy();
    GroupTreeNodePtr fieldNodeCopy = copiedTree->getNode("FIELD");
    GroupTreeNodePtr fieldNodeOriginal = tree->getNode("FIELD");
    BOOST_CHECK(!(fieldNodeCopy == fieldNodeOriginal));
    BOOST_CHECK_EQUAL(fieldNodeCopy->name(), fieldNodeOriginal->name());

    GroupTreeNodePtr L1CHILD1NodeCopy = fieldNodeCopy->getChildGroup("L1CHILD1");
    GroupTreeNodePtr L1CHILD1NodeOriginal = fieldNodeOriginal->getChildGroup("L1CHILD1");
    BOOST_CHECK(!(L1CHILD1NodeCopy == L1CHILD1NodeOriginal));
    BOOST_CHECK_EQUAL(L1CHILD1NodeCopy->name(), L1CHILD1NodeOriginal->name());

    GroupTreeNodePtr L1CHILD2NodeCopy = fieldNodeCopy->getChildGroup("L1CHILD2");
    GroupTreeNodePtr L1CHILD2NodeOriginal = fieldNodeOriginal->getChildGroup("L1CHILD2");
    BOOST_CHECK(!(L1CHILD2NodeCopy == L1CHILD2NodeOriginal));
    BOOST_CHECK_EQUAL(L1CHILD2NodeCopy->name(), L1CHILD2NodeOriginal->name());

    GroupTreeNodePtr L2CHILD1NodeCopy = L1CHILD1NodeCopy->getChildGroup("L2CHILD1");
    GroupTreeNodePtr L2CHILD1NodeOriginal = L1CHILD1NodeOriginal->getChildGroup("L2CHILD1");
    BOOST_CHECK(!(L2CHILD1NodeCopy == L2CHILD1NodeOriginal));
    BOOST_CHECK_EQUAL(L2CHILD1NodeCopy->name(), L2CHILD1NodeOriginal->name());

    GroupTreeNodePtr L2CHILD2NodeCopy = L1CHILD1NodeCopy->getChildGroup("L2CHILD2");
    GroupTreeNodePtr L2CHILD2NodeOriginal = L1CHILD1NodeOriginal->getChildGroup("L2CHILD2");
    BOOST_CHECK(!(L2CHILD2NodeCopy == L2CHILD2NodeOriginal));
    BOOST_CHECK_EQUAL(L2CHILD2NodeCopy->name(), L2CHILD2NodeOriginal->name());

    GroupTreeNodePtr L3CHILD1NodeCopy = L2CHILD1NodeCopy->getChildGroup("L3CHILD1");
    GroupTreeNodePtr L3CHILD1NodeOriginal = L2CHILD1NodeOriginal->getChildGroup("L3CHILD1");
    BOOST_CHECK(!(L3CHILD1NodeCopy == L3CHILD1NodeOriginal));
    BOOST_CHECK_EQUAL(L3CHILD1NodeCopy->name(), L3CHILD1NodeOriginal->name());
}

BOOST_AUTO_TEST_CASE(GetNodes_ReturnsAllNodes) {
    ParserLogPtr parserLog(new ParserLog);
    GroupTreePtr tree(new GroupTree());
    tree->updateTree("L1CHILD1", "FIELD", parserLog, "", -1);
    tree->updateTree("L1CHILD2", "FIELD", parserLog, "", -1);
    tree->updateTree("L2CHILD1", "L1CHILD1", parserLog, "", -1);
    tree->updateTree("L2CHILD2", "L1CHILD1", parserLog, "", -1);
    tree->updateTree("L3CHILD1", "L2CHILD1", parserLog, "", -1);

    std::vector<GroupTreeNodeConstPtr> nodes = tree->getNodes();
    BOOST_CHECK_EQUAL(6U, nodes.size());
    BOOST_CHECK_EQUAL("FIELD", nodes[0U]->name());
    BOOST_CHECK_EQUAL("L1CHILD1", nodes[1U]->name());
    BOOST_CHECK_EQUAL("L2CHILD1", nodes[2U]->name());
    BOOST_CHECK_EQUAL("L3CHILD1", nodes[3U]->name());
    BOOST_CHECK_EQUAL("L2CHILD2", nodes[4U]->name());
    BOOST_CHECK_EQUAL("L1CHILD2", nodes[5U]->name());
}
