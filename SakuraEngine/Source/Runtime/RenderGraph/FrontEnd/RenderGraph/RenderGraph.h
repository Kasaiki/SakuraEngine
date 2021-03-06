﻿/*
 * @Author: your name
 * @Date: 2020-07-30 14:20:24
 * @LastEditTime: 2020-08-21 20:02:16
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \SakuraEngineV3\SakuraEngine\Runtime\RenderGraph\public\RenderGraph.h
 */ 
#pragma once
#include "IRenderDevice.h" 
#include "Module.h"
#include "RenderGraphCore.h"
#include "RuntimeCore/Resource.h"

namespace sakura::graphics
{
    class render_graph_texture;
    class RGHeap;
}

#define constructor_rg_resource_handle(element, funcname)\
[[nodiscard]] element##Handle element##Unsafe()\
{\
	memoryResources.emplace_back(RenderGraphId::uhalf::max/*generation*/);\
	return element##Handle({RenderGraphId::uhalf::max/*generation*/,\
	    static_cast<RenderGraphId::uhalf_t>(memoryResources.size())});\
}\
template<typename... Args>\
[[nodiscard]] element##Handle element##Unsafe(const sakura::string& name, Args&&... args)\
{\
	std::forward<Args...>(args...);\
	auto hdl = element##Unsafe();\
	_blackboard.insert_or_assign(name, hdl);\
	return hdl;\
}\
[[nodiscard]] element##Handle element##Unsafe(const sakura::string& name)\
{\
	auto hdl = element##Unsafe();\
	_blackboard.insert_or_assign(name, hdl);\
	return hdl;\
}\
[[nodiscard]] element##Handle element()\
{\
	memoryResources.emplace_back(0/*generation*/);\
	return element##Handle({0/*generation*/,\
	static_cast<RenderGraphId::uhalf_t>(memoryResources.size())});\
}\
template<typename... Args>\
[[nodiscard]] element##Handle element(const sakura::string& name, Args&&... args)\
{\
	std::forward<Args...>(args...);\
	auto hdl = element();\
	_blackboard.insert_or_assign(name, hdl);\
	return hdl;\
}\
[[nodiscard]] element##Handle element(const sakura::string& name)\
{\
	auto hdl = element();\
	_blackboard.insert_or_assign(name, hdl);\
	return hdl;\
}




#define constructor_rg_object_handle(element, funcname)\
[[nodiscard]] element##Handle element()\
{\
	gpuObjects.emplace_back(0/*generation*/);\
	return element##Handle({0/*generation*/,\
	    static_cast<RenderGraphId::uhalf_t>(gpuObjects.size())});\
}\
template<typename... Args>\
[[nodiscard]] element##Handle element(Args&&... args)\
{\
    auto hdl = element();\
	void(std::forward<Args...>(args...));\
	return hdl;\
}

namespace sakura::graphics
{
    struct RenderPass;
	
    class RenderGraphAPI RenderGraph
    {
    public:
    	struct ResourceNode
    	{
            
    	};
        struct RenderGraphAPI Builder
        {
            friend class RenderGraph;
            friend struct RenderPass;
        	
            Builder(const uint32 generation);
            //void read();
            void write(const Attachment& attachment);
            void write(const RenderPipelineHandle& ppl);

            RenderPipelineHandle pipeline() const;
            Attachment attachment() const;
        protected:
            Attachment _attachment;
            RenderPipelineHandle _pipeline;
            const uint32 generation = 0;
        };
		RenderGraph();
        ~RenderGraph();
		[[nodiscard]] IRenderDevice* get_device(const sakura::string_view name) const;
        void emplace_device(IRenderDevice* device);

        constructor_rg_resource_handle(RenderShader, shader)
        constructor_rg_resource_handle(RenderBuffer, buffer)
        constructor_rg_resource_handle(RenderTexture, texture)
        constructor_rg_resource_handle(RenderAttachment, render_attachment)

        constructor_rg_object_handle(RenderPipeline, render_pipeline)
        constructor_rg_object_handle(SwapChain, swap_chain)
        constructor_rg_object_handle(Fence, fence)

    	template<typename... Args>
        RenderPassHandle create_render_pass(Args&&... args);
        template<typename T, typename... Args>
        RenderPassHandle create_render_pass(Args&&... args);
    	
        RenderPass* render_pass(const RenderPassHandle handle);
        RenderGraph::Builder& builder(const RenderPassHandle handle);

        const RenderResourceHandle blackboard(const sakura::string& name) const;
    	template<typename T>
        const T blackboard(const sakura::string& name) const
    	{
            static_assert(std::is_base_of_v<RenderResourceHandle, T>, "T is not derived from RenderResourceHandle!");
            auto& hdl = blackboard(name);
            return *static_cast<const T*>(&hdl);
    	}
    private:
        RenderDeviceGroupProxy devices;
        sakura::unordered_map<sakura::string, RenderResourceHandle> _blackboard;

    	sakura::vector<uint32> memoryResources;/*generations*/
        sakura::vector<uint32> gpuObjects;/*generations*/
        sakura::vector<sakura::pair<RenderPass*, Builder>> passes;
    };

    struct RenderGraphAPI RenderPass
    {
	virtual ~RenderPass();
	virtual bool execute(const RenderGraph& rg,
          const RenderGraph::Builder& builder, IRenderDevice& device) noexcept = 0;
        virtual bool construct(RenderGraph::Builder& rg) noexcept = 0;
        RenderPassHandle handle() const
        {
            return handle_;
        }
        const RenderCommandBuffer& command_buffer(const int currentCycle = -1) const
        {
            if (!currentCycle || currentCycle > cycle_count_ - 1)
                return command_buffers_[current_cycle_];
            return command_buffers_[current_cycle_];
        }
    	uint8 current_cycle() const
        {
            return current_cycle_;
        }
    	uint8 total_cycle() const
        {
            return cycle_count_;
        }
    protected:
        bool reset();
        RenderCommandBuffer& command_buffer(const int currentCycle = -1)
        {
            if (!currentCycle || currentCycle > cycle_count_ - 1)
                return command_buffers_[current_cycle_];
            return command_buffers_[current_cycle_];
        }
        // TODO: commandBuffer size management.
        RenderCommandBuffer* command_buffers_ = nullptr;
        uint8 current_cycle_ = 0;
        const uint8 cycle_count_ = 0;
        RenderPass(const RenderPassHandle __handle, uint8 cycleCount = 3, size_t bufferSize = 1024);
        RenderPass() = delete;
        const RenderPassHandle handle_;
    };

    struct RenderGraphAPI RenderPassLambda final : public RenderPass
    {
        using Evaluator = sakura::function<bool(IRenderDevice& device, const RenderGraph& rg, RenderCommandBuffer& buffer)>;
        using Constructor = sakura::function<Evaluator(RenderGraph::Builder& builder)>;
        RenderPassLambda(const RenderPassHandle __handle, const Constructor& constructor);

        bool construct(RenderGraph::Builder& builder) noexcept override;
        bool execute(const RenderGraph& rg,
            const RenderGraph::Builder& builder, IRenderDevice& device) noexcept override;
    protected:
        Constructor constructor_;
        mutable Evaluator evaluator_;
    };

    template <typename ... Args>
    RenderPassHandle RenderGraph::create_render_pass(Args&&... args)
    {
        auto newBuilder = RenderGraph::Builder(0);
        auto newHandle = RenderPassHandle(
            RenderGraphId(newBuilder.generation,passes.size())
        );
        passes.emplace_back(sakura::make_pair(
            new RenderPassLambda(newHandle, std::forward<Args>(args)...),
            newBuilder
        ));
        return newHandle;
    }

    template <typename T, typename ... Args>
    RenderPassHandle RenderGraph::create_render_pass(Args&&... args)
    {
        auto newBuilder = RenderGraph::Builder(0);
        auto newHandle = RenderPassHandle(
            RenderGraphId(newBuilder.generation, static_cast<uint32>(passes.size()))
        );
        passes.emplace_back(sakura::make_pair(
            new T(newHandle, std::forward<Args>(args)...),
            newBuilder
        ));
        return newHandle;
    }




	
    class RenderGraphAPI RenderGraphModule : public IModule
    {
    public:
        static RenderGraphModule* create();
	    bool StartUp() override;
	    bool Terminate() override;
    protected:
        static bool render_graph_module_registered_;
    };
}
