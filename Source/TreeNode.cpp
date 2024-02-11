/*
  ==============================================================================

    TreeNode.cpp
    Created: 6 Sep 2021 9:55:33am
    Author:  viktor

  ==============================================================================
*/

#include "TreeNode.h"
TreeNode::TreeNode(Point<float> location, float value)
{
    this->location = location;
    this->value = value;
}

TreeNode::~TreeNode()
{
   /*for (int i = 0; i < this->childNodes.size(); i++)
    {
       if (childNodes[i] != NULL)
       {
           delete this->childNodes[i];
       }
    }*/
}