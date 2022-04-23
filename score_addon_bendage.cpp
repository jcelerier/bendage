#include "score_addon_bendage.hpp"
#include <Bendage/JiPeg.hpp>
#include <Bendage/Xlippy.hpp>
#include <Bendage/SafeWord.hpp>

#include <Avnd/Factories.hpp>
#include <score/plugins/FactorySetup.hpp>
#include <score_plugin_engine.hpp>

/**
 * This file instantiates the classes that are provided by this plug-in.
 */
score_addon_bendage::score_addon_bendage() = default;
score_addon_bendage::~score_addon_bendage() = default;

std::vector<std::unique_ptr<score::InterfaceBase>>
score_addon_bendage::factories(
    const score::ApplicationContext& ctx,
    const score::InterfaceKey& key) const
{
  return Avnd::instantiate_fx<
      Bendage::JiPeg
    , Bendage::Xlippy
    , Bendage::SafeWord
      >(ctx, key);
}

std::vector<score::PluginKey> score_addon_bendage::required() const
{
  return {score_plugin_engine::static_key()};
}

#include <score/plugins/PluginInstances.hpp>
SCORE_EXPORT_PLUGIN(score_addon_bendage)
