
#include <Geode/Geode.hpp>
using namespace geode::prelude;
#include <Geode/modify/EditorUI.hpp>


class $modify(GradientPlusEditorUI, EditorUI) {
	void createGradientFromSelection()
	{
		// Check number of selected objects. If it's 3, run my code. If not return to robtop's code.
		CCArray* selected = this->getSelectedObjects();
		if (selected->count() == 3)
		{
			// Get the game objects for the selected array
			GameObject* obj1 = static_cast<GameObject*>(selected->objectAtIndex(0));
			GameObject* obj2 = static_cast<GameObject*>(selected->objectAtIndex(1));
			GameObject* obj3 = static_cast<GameObject*>(selected->objectAtIndex(2));

			// Assign new group IDs to the objects and keep them in an unordered_set
			std::unordered_set<int> gradientGroups;
			int groupID1 = nextGroupID(m_editorLayer, gradientGroups);
			gradientGroups.insert(groupID1);
			int groupID2 = nextGroupID(m_editorLayer, gradientGroups);
			gradientGroups.insert(groupID2);
			int groupID3 = nextGroupID(m_editorLayer, gradientGroups);
			gradientGroups.insert(groupID3);

			addToGroup(m_editorLayer, obj1, groupID1);
			addToGroup(m_editorLayer, obj2, groupID2);
			addToGroup(m_editorLayer, obj3, groupID3);

			// Find the x position for the gradient trigger (i think robtop does this by taking the leftmost and rightmost corners and finding the average)
			 
			std::unordered_set<float> xPositions;
			xPositions.insert(obj1->getPositionX());
			xPositions.insert(obj2->getPositionX());
			xPositions.insert(obj3->getPositionX());
			auto leftX = std::min_element(xPositions.begin(), xPositions.end());
			float leftXValue = *leftX;
			auto rightX = std::max_element(xPositions.begin(), xPositions.end());
			float rightXValue = *rightX;
			
			// Find the y position for the gradient trigger (this is one tile above the highest corner)
			
			std::unordered_set<float> yPositions;
			yPositions.insert(obj1->getPositionY());
			yPositions.insert(obj2->getPositionY());
			yPositions.insert(obj3->getPositionY());
			auto topY = std::max_element(yPositions.begin(), yPositions.end());
			float topYValue = *topY;

			CCPoint gradientPosition = ccp((leftXValue + rightXValue) * 0.5f, topYValue + 60.0f);
			CCPoint snappedGradientPosition = ccp(std::floor(gradientPosition.x / 30.0f) * 30.0f + 15.0f, std::floor(gradientPosition.y / 30.0f) * 30.0f + 15.0f);
			

			// Create the gradient trigger
			GradientTriggerObject* gradientTrigger = static_cast<GradientTriggerObject*>(createObject(2903, snappedGradientPosition));
			gradientTrigger->setPosition(snappedGradientPosition);

			gradientTrigger->m_vertexMode = true;
			gradientTrigger->m_gradientID = nextGradientID(m_editorLayer);
			gradientTrigger->m_upBottomLeftID = groupID1;
			gradientTrigger->m_downBottomRightID = groupID1;
			gradientTrigger->m_leftTopLeftID = groupID2;
			gradientTrigger->m_rightTopRightID = groupID3;

			// Select gradient and update editor UI
			this->deselectAll();
			this->selectObject(gradientTrigger, true);
			this->updateButtons();
			this->updateObjectInfoLabel();
		}
		else if (selected->count() > 3)
		{
			// Get a list of the selected objects in the order that they were placed (done using the unique ID for each object) to get shape of polygon
			std::vector<GameObject*> selectedObjects;
			for (int i = 0; i < selected->count(); ++i) {
				selectedObjects.push_back(static_cast<GameObject*>(selected->objectAtIndex(i)));
			}
			std::sort(selectedObjects.begin(), selectedObjects.end(), [](GameObject* object1, GameObject* object2) {
				return object1->m_uniqueID < object2->m_uniqueID;
			});

			// Apply new group IDs to the objects
			for (int i = 0; i < selectedObjects.size(); ++i)
			{
				addToGroup(m_editorLayer, selectedObjects[i], nextGroupID(m_editorLayer));
			}

			// Use Delaunay Triangulation and create a list of sets of 3 corners that form valid triangles for the polygon
			// - 


			// Create a gradient for each triangle
		}
		else
		{
			EditorUI::createGradientFromSelection();
		}
	}

	//Find the next free gradient ID (had to do this because getNextFreeGradientID wasn't building on ios or macos)
	static int nextGradientID(LevelEditorLayer* layer) {
		std::unordered_set<int> checkedGroups;
		for (auto obj : CCArrayExt<GameObject*>(layer->m_objects)) {
			if (obj->m_objectID == 2903) {
				auto group = static_cast<GradientTriggerObject*>(obj);
				checkedGroups.insert(group->m_gradientID);
			}
		}
		int nextID = 1;
		while (checkedGroups.contains(nextID)) nextID++;
		return nextID;
	}

	//Add object to the group (had to do this because addObjectToGroup wasn't building on ios or macos)
	static void addToGroup(LevelEditorLayer* layer, GameObject* obj, int groupID) {
		if (obj->addToGroup(groupID) == 1) {
			layer->addToGroup(obj, groupID, false);
		}
	}

	//Find next group ID (had to do this because getNextFreeGroupID wasn't building on ios or macos (wow this has become a recurring pattern))
	static int nextGroupID(LevelEditorLayer* layer, std::unordered_set<int> const& exclude) {
		std::unordered_set<int> checkedGroups = exclude;
		for (auto obj : CCArrayExt<GameObject*>(layer->m_objects)) {
			if (!obj->m_groups) continue;
			for (int i = 0; i < obj->m_groupCount; ++i) {
				checkedGroups.insert((*obj->m_groups)[i]);
			}
		}
		int nextID = 1;
		while (checkedGroups.contains(nextID) && nextID < 9999) nextID++;
		return nextID;
	}

	static int nextGroupID(LevelEditorLayer* layer)
	{
		std::unordered_set<int> checkedGroups;
		for (auto obj : CCArrayExt<GameObject*>(layer->m_objects)) {
			if (!obj->m_groups) continue;
			for (int i = 0; i < obj->m_groupCount; ++i) {
				checkedGroups.insert((*obj->m_groups)[i]);
			}
		}
		int nextID = 1;
		while (checkedGroups.contains(nextID) && nextID < 9999) nextID++;
		return nextID;
	}
};