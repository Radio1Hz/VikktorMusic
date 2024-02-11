/*
  ==============================================================================

    TreeNode.h
    Created: 6 Sep 2021 9:55:33am
    Author:  viktor

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
using namespace juce;

class TreeNode
{
    public:
        TreeNode::TreeNode(Point<float> location, float value);
        TreeNode::~TreeNode();
        Point<float> location;
        float value = 0;
        OwnedArray<TreeNode> childNodes;
};
