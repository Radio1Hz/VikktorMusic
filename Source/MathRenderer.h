/*
  ==============================================================================

	MathRenderer.h
	Created: 27 Aug 2021 9:51:27pm
	Author:  viktor

  ==============================================================================
*/

#pragma once
#pragma once
#include <JuceHeader.h>
#include <math.h>
#include "FunctionProvider.h"
#include "TreeNode.h"
using namespace juce;

class MathRenderer
{

protected:
	juce::Point<float> math_center;
	float math_max_size = 0;
	float area_width = 0;
	float area_height = 0;
	float screen_math_ratio = 0;
	float math_width = 0;
	float math_height = 0;
	float point_size = 5.0f;
	float font_size = 14.0f;

	Point<float> screen_center;
	Point<float> x0;
	Point<float> zeroPoint = Point<float>(0, 0);
	Point<float> math_pointer;
	Random rnd;



public:

	Rectangle<int> area;
	Point<float> screen_pointer = juce::Point<float>(0, 0);
	//OwnedArray<TreeNode> rootNode;
	float lambda = 0; // xn+1 = lambda (1 - xn);
	float start = 0;  // start value
	int iterations = 200;
	int generalIterator = 0;
	int state = 0;
	FunctionProvider function_provider = FunctionProvider();


public:
	MathRenderer(juce::Rectangle<int> area, juce::Point<float> math_center_point, float math_max_size) {
		this->math_center = math_center_point;
		this->math_max_size = math_max_size;
		this->area = area;
		this->area_width = 0;
		this->area_height = 0;
		this->screen_math_ratio = 0;
		this->math_width = 0;
		this->math_height = 0;
		rnd = Random::getSystemRandom();
		//generateCollatzTree(1);
		//generateRandomTree(1);
	}

	~MathRenderer()
	{
		//delete rootNode;
	}

	void MathRenderer::generateRandomTree(int)
	{
		/*int iter = 0;
		rootNode.clear(true);

		TreeNode* temp = rootNode.add(new TreeNode(Point<float>(1.0f, 1.0f), 1.0f));

		while (iter < 1000)
		{
			float val = rnd.nextFloat() * iter;
			temp = temp->childNodes.add(new TreeNode(Point<float>(iter, val), val));
			iter++;
		}*/

	}
	void MathRenderer::generateCollatzTree(int)
	{
		//int iter = 0;

		/*rootNode.clear(true);
		int startInt = start;
		TreeNode* temp = rootNode.add(new TreeNode(Point<float>(1.0f *  iter, 1.0f *  startInt), 1.0f * startInt));
		while (startInt > 1 && iter < 100)
		{
			startInt = function_provider.CollatzConjecture(startInt);
			temp = temp->childNodes.add(new TreeNode(Point<float>(1.0f * iter, 1.0f * startInt), 1.0f));
			iter++;
		}*/

	}
	void MathRenderer::clearTreeNode(TreeNode* node)
	{
		for (int i = 0; i < node->childNodes.size(); i++)
		{
			clearTreeNode(node->childNodes[i]);
		}
		node->childNodes.clear(true);
	}

	void MathRenderer::Draw(Graphics& g, const Rectangle<float> rectarea)
	{
		g.setColour(juce::Colours::lightgrey);
		area_width = (float)rectarea.getWidth();
		area_height = (float)rectarea.getHeight();
		screen_math_ratio = std::min(area_width, area_height) / math_max_size;
		math_width = area_width / screen_math_ratio;
		math_height = area_height / screen_math_ratio;

		screen_center = rectarea.getCentre();
		x0 = juce::Point<float>(screen_center.x - math_center.x * screen_math_ratio, screen_center.y + math_center.y * screen_math_ratio);
		g.setColour(juce::Colours::darkgrey);
		g.drawRect(rectarea, 0.5f);
		DoYourThing(g);

	}

	void MathRenderer::changeListenerCallback(juce::ChangeBroadcaster*)
	{
		generalIterator++;
		generateRandomTree(generalIterator);
	}

	void MathRenderer::SetNewMathCenter(float screen_x, float screen_y)
	{
		this->math_center = ScreenToMathPoint(juce::Point<float>(screen_x, screen_y));
	}

	void MathRenderer::SetNewMathPointer(Point<float> pt)
	{
		this->math_pointer = pt;
	}

	void MathRenderer::TranslateMathCenter(float delta_x_px, float delta_y_px)
	{
		Point<float> transPoint = ScreenToMathPoint(Point<float>(delta_x_px, delta_y_px));
		this->math_center.addXY(transPoint.x, transPoint.y);
	}

	void MathRenderer::SetNewMathSize(float delta)
	{
		this->math_max_size = math_max_size * (1 + delta);
	}

	void MathRenderer::SetNewFontSize(float font_size_new)
	{
		this->font_size = font_size_new;
	}

	Point<float> MathRenderer::ScreenToMathPoint(juce::Point<float> screen_point_par)
	{
		return juce::Point<float>(math_center.x + (screen_point_par.x - screen_center.x) / screen_math_ratio, math_center.y - (screen_point_par.y - screen_center.y) / screen_math_ratio);
	}
private:

	void MathRenderer::DoYourThing(Graphics& g)
	{

		DrawCoordinateSys(g);

		juce::Point<float> res = function_provider.LogisticFunction(lambda, start);

		if (state > 0 && state < 4)
		{
			g.setFont(30.0f);
			g.setColour(juce::Colours::blue);

			DrawLogisticFunction(g, false);
			DrawMathematicalLine(g, juce::Point<float>(start, 0), res, juce::Colours::aqua, true, 0.75f);
			DrawMathematicalLine(g, res, juce::Point<float>(0, res.y), juce::Colours::aqua, true, 0.75f);
			DrawTextMath(g, Point<float>(1.1f, 0.0f), "f(x) = rx(1-x)", juce::Colours::blue);
		}

		if (state > 1 && state < 4)
		{
			DrawLogisticFunction(g, true);
			g.setColour(juce::Colours::red);
			DrawTextMath(g, Point<float>(0.0f, 1.1f), "f(y) = ry(1-y)", juce::Colours::red);
		}

		if (state > 2 && state < 4)
		{
			g.setFont(30.0f);
			g.setColour(juce::Colours::blue);
			DrawLogisticIterations(g);
			DrawTextMath(g, Point<float>(1.1f, 0.0f), "f(x) = rx(1-x)", juce::Colours::blue);
			DrawTextMath(g, Point<float>(0.0f, 1.1f), "f(y) = ry(1-y)", juce::Colours::red);
		}


		if (state > 3)
		{
			res = function_provider.LogisticFunctionSin(lambda, start);
			g.setFont(30.0f);
			g.setColour(juce::Colours::blue);
			g.drawText("f(x) = rsin(x)", area, juce::Justification::centred);
			DrawLogisticFunctionSin(g, false);
		}

		if (state > 4)
		{
			DrawLogisticFunctionSin(g, true);
			DrawLogisticIterationsSin(g);
		}

		DrawPointWithLabel(g, Point<float>(start, 0.0f), "x0");
		DrawPointWithLabel(g, Point<float>(0.0f, res.y), juce::String(res.y));
		DrawPointWithLabel(g, juce::Point<float>(1, 0));
		DrawPointWithLabel(g, juce::Point<float>(0, 1));
		DrawPointWithLabel(g, res);
		DrawPoint(g, juce::Point<float>(0, 1), juce::Colours::lightpink);
	}

	void MathRenderer::DrawTree(Graphics& g, TreeNode* node)
	{
		//float opc = rnd.nextFloat();
		DrawPoint(g, node->location, Colours::lightgrey, 0.5f);
		for (TreeNode* nO : node->childNodes)
		{
			DrawMathematicalLine(g, node->location, nO->location, Colours::lightgrey, false, 0.2f);
			DrawTree(g, nO);
		}
	}

	void MathRenderer::DrawCoordinateSys(Graphics& g)
	{
		g.setColour(juce::Colours::lightgrey);
		Point<float> point1 = zeroPoint;
		Point<float> point2 = zeroPoint;
		point1.addXY(-100.0f, 0);
		point2.addXY(100.0f, 0);
		DrawMathematicalLine(g, point1, point2, juce::Colours::lightgrey, false, 0.2f);
		DrawMathematicalLine(g, point1, point2, juce::Colours::lightgrey, false, 0.2f);
		point1 = zeroPoint;
		point2 = zeroPoint;
		point1.addXY(0, -100.0f);
		point2.addXY(0, 100.0f);
		DrawMathematicalLine(g, point1, point2, juce::Colours::lightgrey, false, 0.2f);
		DrawMathematicalLine(g, point1, point2, juce::Colours::lightgrey, false, 0.2f);
	}



	void MathRenderer::DrawLogisticIterationsSin(Graphics& g)
	{
		float start_x = start;
		juce::Line<float> line_segment = juce::Line<float>(MathToScreenPoint(juce::Point<float>(start_x, 0.0f)), juce::Point<float>(0.0f, 0.0f));
		juce::Point<float> math_point = juce::Point<float>(start_x, 0.0f);

		for (int i = 0; i < iterations; i++)
		{
			math_point = function_provider.LogisticFunctionSin(lambda, math_point.x);
			DrawPoint(g, math_point, juce::Colours::lightblue);

			line_segment.setEnd(MathToScreenPoint(math_point));
			g.setColour(juce::Colours::lightgrey);
			g.drawLine(line_segment, 1.0);

			line_segment.setStart(MathToScreenPoint(math_point));

			math_point = function_provider.LogisticFunctionInverseSin(lambda, math_point.y);
			DrawPoint(g, math_point, juce::Colours::lightpink);
			line_segment.setEnd(MathToScreenPoint(math_point));
			g.setColour(juce::Colours::lightgrey);
			g.drawLine(line_segment, 1.0);
			line_segment.setStart(MathToScreenPoint(math_point));

		}
	}
	void MathRenderer::DrawLogisticIterations(Graphics& g)
	{
		float start_x = start;
		juce::Line<float> line_segment = juce::Line<float>(MathToScreenPoint(juce::Point<float>(start_x, 0.0f)), juce::Point<float>(0.0f, 0.0f));
		juce::Point<float> math_point = juce::Point<float>(start_x, 0.0f);

		for (int i = 0; i < iterations; i++)
		{
			math_point = function_provider.LogisticFunction(lambda, math_point.x);
			if (abs(MathToScreenPoint(math_point).x) < 10000 && abs(MathToScreenPoint(math_point).y) < 10000)
			{
				DrawPoint(g, math_point, juce::Colours::lightblue);

				line_segment.setEnd(MathToScreenPoint(math_point));
				g.setColour(juce::Colours::lightgrey);
				g.drawLine(line_segment, 1.0);

				line_segment.setStart(MathToScreenPoint(math_point));

				math_point = function_provider.LogisticFunctionInverse(lambda, math_point.y);
				if (abs(MathToScreenPoint(math_point).x) < 10000 && abs(MathToScreenPoint(math_point).y) < 10000)
				{
					DrawPoint(g, math_point, juce::Colours::lightpink);
					line_segment.setEnd(MathToScreenPoint(math_point));
					g.setColour(juce::Colours::lightgrey);
					g.drawLine(line_segment, 1.0);
					line_segment.setStart(MathToScreenPoint(math_point));
				}
			}

			if (i > iterations * 0.9f)
			{
				DrawPoint(g, juce::Point<float>(0, math_point.y), juce::Colours::yellow);
			}
		}

	}

	void MathRenderer::DrawLogisticFunction(Graphics& g, bool inverse = false)
	{
		float step = 10;
		float math_x = 0, math_y = 0;

		juce::Point<float> path_point = juce::Point<float>(-1000, -1000);


		if (inverse)
		{
			for (float y = 0; y < area_height; y += step)
			{
				math_y = -(y - x0.y) / screen_math_ratio;
				DrawSegment(g, function_provider.LogisticFunctionInverse(lambda, math_y), path_point, juce::Colours::red);
			}
		}
		else
		{
			for (float x = 0; x < area_width; x += step)
			{
				math_x = (x - x0.x) / screen_math_ratio;
				DrawSegment(g, function_provider.LogisticFunction(lambda, math_x), path_point, juce::Colours::blue);
			}
		}

	}

	void MathRenderer::DrawLogisticFunctionSin(Graphics& g, bool inverse = false)
	{
		float step = 10;
		float math_x = 0, math_y = 0;

		juce::Point<float> path_point = juce::Point<float>(-1000, -1000);


		if (inverse)
		{
			for (float y = 0; y < area_height; y += step)
			{
				math_y = -(y - x0.y) / screen_math_ratio;
				DrawSegment(g, function_provider.LogisticFunctionInverseSin(lambda, math_y), path_point, juce::Colours::red);
			}
		}
		else
		{
			for (float x = 0; x < area_width; x += step)
			{
				math_x = (x - x0.x) / screen_math_ratio;
				DrawSegment(g, function_provider.LogisticFunctionSin(lambda, math_x), path_point, juce::Colours::blue);
			}
		}

	}



protected:
	void MathRenderer::DrawSegment(Graphics& g, juce::Point<float> math_point_par, juce::Point<float>& path_point_par, juce::Colour col)
	{
		juce::Point<float> screen_p = MathToScreenPoint(math_point_par);
		if (screen_p.x > 0 && screen_p.x < area_width && screen_p.y < area_height && screen_p.y > 0)
		{
			if (path_point_par.x == -1000 && path_point_par.y == -1000)
			{
				path_point_par.x = screen_p.x;
				path_point_par.y = screen_p.y;
			}
			else
			{
				g.setColour(col);
				g.drawLine(juce::Line<float>(path_point_par, screen_p), 2.0);
				path_point_par.x = screen_p.x;
				path_point_par.y = screen_p.y;
			}
		}
	}

	void MathRenderer::DrawTextMath(Graphics& g, juce::Point<float> math_point_par, String label, juce::Colour col)
	{
		g.setColour(col);
		g.setFont(16.0f);
		juce::Point<float> screen_p = MathToScreenPoint(math_point_par);

		g.drawText(label, juce::Rectangle<float>(screen_p.x, screen_p.y, 200, 20), juce::Justification::centredLeft);
	}
	void MathRenderer::DrawPointWithLabel(Graphics& g, juce::Point<float> math_point_par, String label = "")
	{
		g.setColour(juce::Colours::lightgrey);
		juce::Rectangle<float> pointArea(point_size, point_size);
		juce::String math_pointer_label = juce::String("(");
		if (label != "")
		{
			math_pointer_label = label;
		}
		else
		{
			math_pointer_label.append(juce::String(math_point_par.x), 8);
			math_pointer_label.append(", ", 2);
			math_pointer_label.append(juce::String(math_point_par.y), 8);
			math_pointer_label.append(")", 1);
		}


		juce::Point<float> screen_p = MathToScreenPoint(math_point_par);
		pointArea.setCentre(screen_p);
		FontOptions fOpt("Courier New", 14.80f, juce::Font::plain);
		juce::Font f = juce::Font(fOpt);
		g.setFont(14.0f);
		g.drawEllipse(pointArea, 1.0f);
		g.setColour(juce::Colours::darkgrey);
		StringRef str(math_pointer_label);
		g.fillRoundedRectangle(juce::Rectangle<float>(screen_p.x, screen_p.y, TextLayout::getStringWidth(f,str), 20), 5);
		g.setColour(juce::Colours::white);
		g.drawText(math_pointer_label, juce::Rectangle<float>(screen_p.x, screen_p.y, 200, 20), juce::Justification(0));
	}

	void MathRenderer::DrawPoint(Graphics& g, juce::Point<float> math_point_par, juce::Colour col = juce::Colours::lightgrey, float opacity = 1.0f, float diameter = 3.0f)
	{
		g.setColour(col);
		juce::Rectangle<float> pointArea(diameter, diameter);
		juce::Point<float> screen_p = MathToScreenPoint(math_point_par);

		if (screen_p.x > 0 && screen_p.x < area_width && screen_p.y < area_height && screen_p.y > 0)
		{
			g.setOpacity(opacity);
			pointArea.setCentre(screen_p);
			g.drawEllipse(pointArea, 1.0f);
		}
	}

	void MathRenderer::DrawMathematicalLine(Graphics& g, juce::Point<float> start_point, juce::Point<float> end, juce::Colour col, bool dashed = false, float opacity = 1.0f)
	{
		g.setColour(col);
		juce::Line<float> screenLine = juce::Line<float>(MathToScreenPoint(start_point), MathToScreenPoint(end));
		float dashPattern[2];
		dashPattern[0] = 2.0;
		dashPattern[1] = 4.0;
		g.setOpacity(opacity);

		if (dashed)
		{
			g.drawDashedLine(screenLine, dashPattern, 2, 0.5f, 0);
		}
		else
		{
			g.drawLine(screenLine);
		}
	}

	juce::Point<float> MathRenderer::MathToScreenPoint(juce::Point<float> math_point_par)
	{
		juce::Point<float> ret;
		ret = juce::Point<float>(x0.x + math_point_par.x * screen_math_ratio, x0.y - math_point_par.y * screen_math_ratio);

		//if (ret.x < this->area.getX())
		//{
		//    ret.x = this->area.getX();
		//}

		//if (ret.x > this->area.getX())
		//{
		//    ret.x = this->area.getX();
		//}

		//if (ret.x > screen_width)
		//{
		//    ret.x = screen_width;
		//}

		//if (ret.y < this->area.getY())
		//{
		//    ret.y = this->area.getY();
		//}

		//if (ret.y > this->area.getY())
		//{
		//    ret.y = this->area.getY();
		//}

		//if (ret.y > screen_height)
		//{
		//    ret.y = screen_height;
		//}
		return ret;
	}


};