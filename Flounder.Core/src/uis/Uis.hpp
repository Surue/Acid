#pragma once

#include "../engine/Engine.hpp"
#include "../fonts/text.hpp"
#include "../guis/gui.hpp"

#include "imanageruis.hpp"
#include "uiselector.hpp"
#include "containerscreen.hpp"

namespace Flounder
{
	/// <summary>
	/// A module used for that manages gui textures in a container.
	/// </summary>
	class uis :
		public IModule
	{
	private:
		IManagerUis *m_managerUis;
		uiselector *m_selector;
		UiObject *m_container;
		std::vector<UiObject*> *m_objects;
	public:
		FontType *m_arial;
		FontType *m_berlinSans;
		FontType *m_cafeFrancoise;
		FontType *m_candara;
		FontType *m_segoe;

		/// <summary>
		/// Gets this engine instance.
		/// </summary>
		/// <returns> The current module instance. </returns>
		static uis *get()
		{
			return static_cast<uis*>(Engine::Get()->GetModule("uis"));
		}

		/// <summary>
		/// Creates a new uis module.
		/// </summary>
		uis();

		/// <summary>
		/// Deconstructor for the uis module.
		/// </summary>
		~uis();

		void Update() override;

		/// <summary>
		/// Gets the uis manager.
		/// </summary>
		/// <returns> The uis manager. </returns>
		IManagerUis *getManager() { return m_managerUis; };

		/// <summary>
		/// Sets the current uis manager to a new uis manager.
		/// </summary>
		/// <param name="managerUis"> The new uis manager. </param>
		void setMaster(IManagerUis *managerUis) { m_managerUis = managerUis; }

		/// <summary>
		/// Gets the screen container.
		/// </summary>
		/// <returns> The screen container. </returns>
		UiObject *getContainer() { return m_container; }

		/// <summary>
		/// Gets the main GUI selector.
		/// </summary>
		/// <returns> The GUI selector. </returns>
		uiselector *getSelector() { return m_selector; }

		/// <summary>
		/// The rendering objects from the container. Updated each update.
		/// </summary>
		/// <returns> The objects. </returns>
		std::vector<UiObject*> *getObjects() { return m_objects; };
	};
}