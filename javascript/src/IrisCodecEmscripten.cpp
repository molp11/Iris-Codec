#include <emscripten/bind.h>
#include <emscripten/val.h>
#include "IrisCodecPriv.hpp"

using namespace emscripten;

// Version information
val getCodecVersion()
{
    IrisCodec::Version version = {
        CODEC_MAJOR_VERSION,
        CODEC_MINOR_VERSION,
        CODEC_BUILD_NUMBER};

    val result = val::object();
    result.set("major", version.major);
    result.set("minor", version.minor);
    result.set("build", version.build);
    return result;
}

// Validate a slide file
val validateSlide(const std::string &filePath)
{
    Iris::Result result = IrisCodec::validate_slide({.filePath = filePath,
                                                     .context = nullptr,
                                                     .writeAccess = false});

    val jsResult = val::object();
    jsResult.set("success", result == Iris::IRIS_SUCCESS);
    jsResult.set("message", result.message);
    return jsResult;
}

// Get slide info
val getSlideInfo(const std::string &filePath)
{
    // Open the slide
    auto slide = IrisCodec::open_slide({.filePath = filePath,
                                        .context = nullptr,
                                        .writeAccess = false});

    if (!slide)
    {
        val jsResult = val::object();
        jsResult.set("success", false);
        jsResult.set("message", "Failed to open slide");
        return jsResult;
    }

    // Get slide info
    IrisCodec::SlideInfo info;
    Iris::Result result = IrisCodec::get_slide_info(slide, info);

    val jsResult = val::object();
    jsResult.set("success", result == Iris::IRIS_SUCCESS);

    if (result == Iris::IRIS_SUCCESS)
    {
        val extent = val::object();
        extent.set("width", info.extent.width);
        extent.set("height", info.extent.height);

        // Create an array of layer information
        val layers = val::array();
        for (size_t i = 0; i < info.extent.layers.size(); i++)
        {
            const auto &layer = info.extent.layers[i];
            val layerInfo = val::object();
            layerInfo.set("xTiles", layer.xTiles);
            layerInfo.set("yTiles", layer.yTiles);
            layerInfo.set("scale", layer.scale);
            layerInfo.set("downsample", layer.downsample);
            layers.call<void>("push", layerInfo);
        }

        extent.set("layers", layers);
        extent.set("layerCount", info.extent.layers.size());

        jsResult.set("extent", extent);
        jsResult.set("encoding", static_cast<int>(info.encoding));

        // Add metadata if available
        if (info.metadata.magnification > 0)
        {
            val metadata = val::object();
            metadata.set("magnification", info.metadata.magnification);
            metadata.set("micronsPerPixel", info.metadata.micronsPerPixexl);
            jsResult.set("metadata", metadata);
        }
    }
    else
    {
        jsResult.set("message", result.message);
    }

    return jsResult;
}

// Core slide reading functionality
val readSlideTile(const std::string &filePath, int layerIndex, int tileIndex)
{
    // Open the slide
    auto slide = IrisCodec::open_slide({.filePath = filePath,
                                        .context = nullptr,
                                        .writeAccess = false});

    if (!slide)
    {
        val result = val::object();
        result.set("success", false);
        result.set("message", "Failed to open slide");
        return result;
    }

    // Read the tile
    Iris::Buffer tileData = IrisCodec::read_slide_tile({
        .slide = slide,
        .layerIndex = static_cast<unsigned>(layerIndex),
        .tileIndex = static_cast<unsigned>(tileIndex),
        .desiredFormat = Iris::FORMAT_R8G8B8A8
    });

    if (!tileData)
    {
        val result = val::object();
        result.set("success", false);
        result.set("message", "Failed to read tile");
        return result;
    }

    // Create a TypedArray from the buffer data
    val uint8Array = val::global("Uint8Array").new_(tileData->size());
    
    // Copy the data directly instead of using the memory view
    for (size_t i = 0; i < tileData->size(); i++) {
        uint8Array.set(i, static_cast<uint8_t*>(tileData->data())[i]);
    }

    val result = val::object();
    result.set("success", true);
    result.set("data", uint8Array);
    result.set("dataSize", tileData->size());
    result.set("width", 256); // TILE_PIX_LENGTH
    result.set("height", 256); // TILE_PIX_LENGTH
    
    // Include encoding information
    IrisCodec::SlideInfo info;
    IrisCodec::get_slide_info(slide, info);
    result.set("encoding", static_cast<int>(info.encoding));
    
    return result;
}

// Simple binding to test compilation
EMSCRIPTEN_BINDINGS(iris_codec) {
    function("getCodecVersion", &getCodecVersion);
    function("validateSlide", &validateSlide);
    function("getSlideInfo", &getSlideInfo);
    function("readSlideTile", &readSlideTile);
}

