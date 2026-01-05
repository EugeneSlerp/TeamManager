# TeamManager

**TeamManager** is a flexible Unreal Engine plugin for managing factions and teams in your project.  
The plugin allows defining relationships between objects and teams, enabling gameplay logic such as allies, enemies, and AI interactions.

---

## 📌 Overview

TeamManager provides a faction/team system that can be integrated as a plugin into any Unreal Engine project.  
It is designed to give a simple and extensible foundation for:

- friendly/enemy relationships between characters,  
- ally and enemy identification,  
- integration with AI, combat, and gameplay systems.

---

## 🚀 Features

- Easy integration as an Unreal Engine plugin  
- Faction / Team system  
- API to check relationships between objects  
- Extendable logic in C++ and Blueprints  

---

## 📁 Repository Structure

TeamManager/
├── Config/
├── Resources/
├── Source/ FlexibleTeamManager/
├── FlexibleTeamManager.uplugin


---

## 💡 Installation

1. **Clone the repository:**
```sh
git clone https://github.com/EugeneSlerp/TeamManager.git

    Copy the FlexibleTeamManager folder into your Unreal project’s Plugins directory:

YourProject/Plugins/FlexibleTeamManager

    Open your project in Unreal Engine 5.

        Confirm compilation if prompted.

        The plugin will appear in the list of available plugins.

    Make sure your project uses UE5, as this plugin is designed for Unreal Engine 5.

📘 Usage Example

In your project’s code, you can use the TeamManager API to check object relationships:

// C++ example
if (TeamManager::IsFriendly(ActorA, ActorB)) {
    // logic for allies
}

if (TeamManager::IsEnemy(ActorA, ActorB)) {
    // logic for enemies
}

(You can add specific function examples from your code for clarity.)
🛠️ Build

The plugin builds using standard Unreal Engine tools.
Open your project in the editor and allow UE to rebuild the plugin on first launch.
🧠 Why Use This Plugin

TeamManager allows you to:

    standardize a faction system across UE projects,

    avoid duplicating logic in each new project,

    quickly check relationships between objects,

    easily extend the logic for AI, combat, dialogue, etc.

📄 License

MIT License © 2025 EugeneSlerp
📌 Feedback / Contributions

If you use this plugin or want to improve it, feel free to open issues or submit Pull Requests.
