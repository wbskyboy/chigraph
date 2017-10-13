/// \file ModuleProvider.hpp

#pragma once

#ifndef CHI_MODULE_PROVIDER_HPP
#define CHI_MODULE_PROVIDER_HPP

#include "chi/Fwd.hpp"

#include <boost/filesystem/path.hpp>
#include <ctime>

#include "chi/Support/json.hpp"

namespace chi {

/// This class  provides an interface for fetching and caching compiled modules
struct ModuleProvider {
	/// Create a module cache
	explicit ModuleProvider() {}

	/// Destructor
	virtual ~ModuleProvider() = default;

	// No copy or move
	ModuleProvider(const ModuleProvider&) = delete;
	ModuleProvider(ModuleProvider&&)      = delete;
	ModuleProvider& operator=(const ModuleProvider&) = delete;
	ModuleProvider& operator=(ModuleProvider&&) = delete;

	/// Read a module from disk
	/// Assumes all dependencies are loaded, will error if not
	/// \param module The name of the module to load
	/// \param toFill The JSON object to fill
	/// \param toFillTime The time of that last edit of the module
	/// \return The Result
	virtual Result loadModule(const boost::filesystem::path& module, nlohmann::json* toFill,
	                          std::time_t* toFillTime) = 0;

	/// Save a module to disk
	/// \param modToSave the module to save
	/// \return The Result
	virtual Result saveModule(const GraphModule& modToSave) const = 0;

	/// Get the path to the C sources of a module
	/// \param module The module to get the c source path
	/// \return The path to the C sources
	virtual boost::filesystem::path pathToCSources(const GraphModule& module) const = 0;
	/// List all the modules in the workspace
	/// \return All the modules
	virtual std::vector<boost::filesystem::path> listModules() const = 0;

	/// Peek at a module's dependencies without loading it
	/// \param module The  module to get the dependencies
	/// \return The list of direct dependencies (not a recursive search)
	virtual std::vector<boost::filesystem::path> peekDepenencies(
	    const boost::filesystem::path& module) const = 0;

	/// Cache a module
	/// \param moduleName The name of the module to cache
	/// \pre `!moduleName.empty()`
	/// \param compiledModule The IR that's been compiled from this module
	/// \param timeAtFileRead The time to store as the cache time. Should be the time the module
	/// was read from disk \return The Result
	virtual Result cacheModule(const boost::filesystem::path& moduleName,
	                           llvm::Module& compiledModule, std::time_t timeAtFileRead) = 0;

	/// Inavlidate the cache, ie. delete the cache file
	/// \param moduleName The name of the module to invalidate
	/// \pre `!moduleName.empty()`
	virtual void invalidateCache(const boost::filesystem::path& moduleName) = 0;

	/// Get the time that a cache was updated
	/// \param moduleName The module to get the last update time for
	/// \return The time it was updated, or 0 if there is no cache
	virtual std::time_t cacheUpdateTime(const boost::filesystem::path& moduleName) const = 0;

	/// Retrieve a module from the cache
	/// \param moduleName The name of the module to retrieve
	/// \pre `!moduleName.empty()`
	/// \param atLeastThisNew Make sure the cache is at least as new as this
	/// \return A llvm::Module, or nullptr if no suitable cache was found
	virtual std::unique_ptr<llvm::Module> retrieveFromCache(
	    const boost::filesystem::path& moduleName, std::time_t atLeastThisNew) = 0;
};

}  // namespace chi

#endif  // CHI_MODULE_CACHE_HPP
