#include "remote_scales_plugin_registry.h"

// ---------------------------------------------------------------------------------------
// ------------------------   RemoteScalesPluginRegistry    -------------------------------
// ---------------------------------------------------------------------------------------
RemoteScalesPluginRegistry* RemoteScalesPluginRegistry::instance = nullptr;

void RemoteScalesPluginRegistry::registerPlugin(RemoteScalesPlugin plugin) {
  // Check if a plugin with the same ID already exists
  Serial.println("registering plugin..");
  for (const auto& existingPlugin : plugins) {
    if (existingPlugin.id == plugin.id) {
      return;
    }
  }

  plugins.push_back(plugin);
  Serial.println("plugin registration successful!");
}

bool RemoteScalesPluginRegistry::containsPluginForDevice(const DiscoveredDevice& device) {
  for (const auto& plugin : plugins) {
    if (plugin.handles(device)) {
      return true;
    }
  }
  return false;
}

std::unique_ptr<RemoteScales> RemoteScalesPluginRegistry::initialiseRemoteScales(const DiscoveredDevice& device) {
  for (const auto& plugin : plugins) {
    if (plugin.handles(device)) {
      return plugin.initialise(device);
    }
  }
  return nullptr;
}
