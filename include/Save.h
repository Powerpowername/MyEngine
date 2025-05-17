#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION	// include之前必须定义
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

/**
 * 图像缓冲区类，用于存储和操作图像数据
 */
struct ImageBuf
{
	typedef unsigned int u32;

	int             Width, Height;	// 图像的宽高
	u32*            Data;			// 图像数据存储区，每个像素占4字节(RGBA)

	ImageBuf() { Width = Height = 0; Data = NULL; }
	~ImageBuf() { Clear(); }
	
	/** 清理图像数据，释放内存 */
	void Clear() { if (Data) free(Data); Data = NULL; }

	/**
	 * 创建空图像缓冲区
	 * @param w 图像宽度
	 * @param h 图像高度
	 */
	void CreateEmpty(int w, int h)
	{
		Clear();
		Width = w;
		Height = h;
		Data = (u32*)malloc(Width * Height * 4);
		memset(Data, 0, Width * Height * 4);
	}
	
	/**
	 * 从OpenGL帧缓冲区捕获图像
	 * @param x 捕获区域左上角x坐标
	 * @param y 捕获区域左上角y坐标
	 * @param w 捕获区域宽度
	 * @param h 捕获区域高度
	 */
	void CreateFromCaptureGL(int x, int y, int w, int h)
	{
		Clear();
		Width = w;
		Height = h;
		Data = (u32*)malloc(Width * Height * 4);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);	// 设置像素存储模式
		glReadPixels(x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, Data);	// 从帧缓冲区读取像素数据
		RemoveAlpha();	// 处理Alpha通道
	}

	/**
	 * 将图像保存为PNG文件
	 * @param filename 文件名
	 */
	void SaveFile(const char* filename)
	{
		stbi_write_png(filename, Width, Height, 4, Data, Width * 4);
	}

	/**
	 * 处理Alpha通道，确保所有像素都是不透明的(Alpha=255)
	 */
	void RemoveAlpha()
	{
		u32* p = Data;
		int n = Width * Height;
		while (n-- > 0)
		{
			*p |= 0xFF000000;	// 设置Alpha通道为255(不透明)
			p++;
		}
	}

	/**
	 * 将当前图像的一部分复制到目标图像缓冲区
	 * @param dst 目标图像缓冲区
	 * @param src_x 源图像区域左上角x坐标
	 * @param src_y 源图像区域左上角y坐标
	 * @param dst_x 目标图像区域左上角x坐标
	 * @param dst_y 目标图像区域左上角y坐标
	 * @param w 区域宽度
	 * @param h 区域高度
	 */
	void BlitTo(ImageBuf* dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h)
	{
		ImageBuf* src = this;
		IM_ASSERT(dst != src);	// 确保源和目标不是同一个缓冲区
		IM_ASSERT(dst != NULL);
		IM_ASSERT(src_x >= 0 && src_y >= 0);
		IM_ASSERT(src_x + w <= src->Width);
		IM_ASSERT(src_y + h <= src->Height);
		IM_ASSERT(dst_x >= 0 && dst_y >= 0);
		IM_ASSERT(dst_x + w <= dst->Width);
		IM_ASSERT(dst_y + h <= dst->Height);
		
		// 逐行复制像素数据
		for (int y = 0; y < h; y++)
			memcpy(dst->Data + dst_x + (dst_y + y) * dst->Width, 
				   src->Data + src_x + (src_y + y) * src->Width, w * 4);
	}

	/**
	 * 垂直翻转图像(上下颠倒)
	 */
	void FlipVertical()
	{
		int comp = 4;	// 每个像素4个通道(RGBA)
		int stride = Width * comp;	// 每行字节数
		unsigned char* line_tmp = new unsigned char[stride];	// 临时行缓冲区
		unsigned char* line_a = (unsigned char*)Data;	// 指向第一行
		unsigned char* line_b = (unsigned char*)Data + (stride * (Height - 1));	// 指向最后一行
		
		// 交换顶部和底部的行，向中间移动直到相遇
		while (line_a < line_b)
		{
			memcpy(line_tmp, line_a, stride);
			memcpy(line_a, line_b, stride);
			memcpy(line_b, line_tmp, stride);
			line_a += stride;	// 指向下一行
			line_b -= stride;	// 指向上一行
		}
		delete[] line_tmp;
	}
	
	/**
	 * 获取指定位置像素的指针
	 * @param x 像素x坐标
	 * @param y 像素y坐标
	 * @return 像素数据指针
	 */
	u32* GetPtr(int x, int y)
	{
		return &Data[x + y * Width];
	}
	
	/**
	 * 获取指定位置像素的值
	 * @param x 像素x坐标
	 * @param y 像素y坐标
	 * @return 像素值(32位无符号整数)
	 */
	u32 GetPixel(int x, int y) const
	{
		return Data[x + y * Width];
	}
};

/**
 * 截图工具类，用于捕获和处理ImGui界面的截图
 */
struct ScreenshotMaker
{
	ImGuiStorage    Storage;			// 用于存储窗口选择状态等数据
	float           BorderSize;			// 截图边框大小
	bool            LargePictureMode;	// 是否使用大图片模式(用于捕获完整窗口)
	ImRect          VisibleRect;		// 可见区域矩形
	bool            CaptureActive;		// 是否正在进行截图
	int             CaptureFrame;		// 截图处理的当前帧
	ImRect          CaptureRect;		// 截图区域矩形
	int             CaptureLargeOffY;	// 大图片模式下的Y偏移量
	ImGuiWindow*    CaptureWindow;		// 当前正在捕获的窗口
	ImageBuf        Output;				// 输出图像缓冲区

	ScreenshotMaker()
	{
		BorderSize = 16.0f;
		LargePictureMode = false;
		CaptureActive = false;
		CaptureFrame = 0;
		CaptureLargeOffY = 0;
		CaptureWindow = NULL;
	}

	/**
	 * 开始截图过程
	 * @param window 要捕获的窗口，如果为NULL则捕获可见区域
	 */
	void CaptureStart(ImGuiWindow* window)
	{
		CaptureActive = true;
		CaptureFrame = 0;
		Output.Clear();
		CaptureWindow = window;
		if (!CaptureWindow)
			LargePictureMode = false;

		if (LargePictureMode)
		{
			// 调整窗口大小以适应内容
			ImGui::SetWindowSize(CaptureWindow->Name, ImVec2(CaptureWindow->SizeFull.x, CaptureWindow->ContentSize.y));
			// 创建足够大的输出缓冲区来容纳整个窗口
			Output.CreateEmpty((int)(CaptureWindow->Size.x + BorderSize * 2), (int)(CaptureWindow->Size.y + BorderSize * 2));
		}
		else
		{
			// 非大图片模式，立即捕获可见区域
			ImGuiIO& io = ImGui::GetIO();
			// 从OpenGL帧缓冲区捕获图像
			Output.CreateFromCaptureGL((int)CaptureRect.Min.x, (int)io.DisplaySize.y - (int)CaptureRect.Max.y, 
									  (int)CaptureRect.GetWidth(), (int)CaptureRect.GetHeight());
			Output.FlipVertical();	// 垂直翻转图像(OpenGL坐标系统与常规图像坐标系统不同)
			CaptureEnd();	// 完成截图
			return;
		}
	}

	/**
	 * 更新截图过程，在大图片模式下分块捕获窗口内容
	 */
	void CaptureUpdate()
	{
		if (!LargePictureMode)
			return;

		int TIME_STEP = 15;	// 每15帧处理一次，控制捕获速度
		IM_ASSERT(TIME_STEP > 5);

		int chunk_no = CaptureFrame / TIME_STEP;	// 当前处理的块编号
		int chunk_w = (int)CaptureRect.GetWidth();	// 每块的宽度
		int chunk_h = (int)CaptureRect.GetHeight();	// 每块的高度

		if ((CaptureFrame % TIME_STEP) == 2)
		{
			// 移动窗口到适当位置以便捕获下一块
			ImVec2 p = ImVec2(BorderSize+CaptureRect.Min.x, BorderSize+CaptureRect.Min.y);
			p.y -= chunk_no * CaptureRect.GetHeight();	// 计算Y偏移量
			ImGui::SetWindowPos(CaptureWindow->Name, p);	// 设置窗口位置
		}

		if ((CaptureFrame % TIME_STEP) == 4)
		{
			// 捕获当前块并拼接到输出图像
			ImGuiIO& io = ImGui::GetIO();
			ImageBuf img;
			// 从OpenGL帧缓冲区捕获当前块
			img.CreateFromCaptureGL((int)CaptureRect.Min.x, (int)io.DisplaySize.y - (int)CaptureRect.Max.y, chunk_w, chunk_h);
			img.FlipVertical();	// 垂直翻转图像
			int chunk_y = chunk_no * chunk_h;	// 当前块在输出图像中的Y位置
			// 将当前块复制到输出图像的适当位置
			img.BlitTo(&Output, 0, 0, 0, chunk_y, chunk_w, ImMin(chunk_h, Output.Height - chunk_y));
			
			// 调试用：保存每个块为单独的PNG文件
			/*char buf[128];
			sprintf_s(buf, "out%03d.png", chunk_no);
			img.SaveFile(buf);*/
			
			// 检查是否已经捕获完整个窗口
			if (CaptureWindow->Pos.y + CaptureWindow->SizeFull.y < CaptureRect.Max.y)
				CaptureEnd();	// 如果窗口已完全捕获，则结束截图过程
		}

		CaptureFrame++;	// 增加帧计数
	}

	/**
	 * 结束截图过程并保存结果
	 */
	void CaptureEnd()
	{
		if (Output.Data)
			Output.SaveFile("out.png");	// 保存最终截图为PNG文件
		CaptureActive = false;	// 标记截图过程结束
	}

	/**
	 * 绘制截图工具的用户界面
	 * @param p_open 指向bool的指针，用于控制窗口是否可见
	 */
	void Draw(bool* p_open = NULL)
	{
		if (!ImGui::Begin("Screenshot Maker", p_open))
		{
			ImGui::End();
			return;
		}

		ImGuiContext& g = *GImGui;
		ImGuiIO& io = ImGui::GetIO();

		// 截图按钮和状态显示
		bool capture = ImGui::Button("ALT+C: Capture");
		if (CaptureActive)
		{
			ImGui::SameLine();
			ImGui::Text("Capturing...");	// 显示正在捕获的状态
		}
		
		// 截图设置选项
		ImGui::SliderFloat("Border", &BorderSize, 0.0f, 32.0f, "%.0f");	// 调整边框大小
		ImGui::Checkbox("Large Window Capture Mode (1 window)", &LargePictureMode);	// 启用/禁用大图片模式
		ImGui::Separator();

		ImVector<ImGuiWindow*> windows_selected;	// 存储被选中的窗口
		VisibleRect = ImRect(ImVec2(0, 0), io.DisplaySize);	// 设置可见区域为整个显示区域
		ImRect capture_rect;	// 截图区域矩形

		ImGui::Text("Windows:");
		// 显示所有可捕获的窗口列表
		for (int i = 0; i < g.Windows.size(); i++)
		{
			ImGuiWindow* window = g.Windows[i];
			if (!window->Active && !window->WasActive)
				continue;
			if (window->Flags & ImGuiWindowFlags_Popup)
			{
				CaptureRect.Add(window->Rect());	// 将弹出窗口添加到截图区域
				continue;
			}
			if (window->Flags & ImGuiWindowFlags_ChildWindow)
				continue;	// 不处理子窗口

			ImGui::PushID(window);
			// 获取或设置窗口选择状态
			bool* p_selected = (bool*)Storage.GetIntRef(window->RootWindow->ID, 0);
			ImGui::Checkbox("##checkbox", p_selected);	// 选择窗口的复选框
			ImGui::SameLine();
			ImGui::TextUnformatted(window->Name);	// 显示窗口名称
			
			if (*p_selected)
			{
				capture_rect.Add(window->Rect());	// 将选中的窗口添加到截图区域
				windows_selected.push_back(window);	// 添加到选中窗口列表
			}
			
			// 窗口位置和大小调整控件
			ImGui::SameLine(160);
			ImGui::PushItemWidth(120);
			ImGui::DragFloat2("Pos", &window->Pos.x, 0.05f, 0.0f, 0.0f, "%.0f");	// 拖动调整窗口位置
			ImGui::SameLine();
			ImGui::DragFloat2("Size", &window->SizeFull.x, 0.05f, 0.0f, 0.0f, "%.0f");	// 拖动调整窗口大小
			ImGui::PopItemWidth();
			ImGui::PopID();
		}

		// 计算最终的截图区域，添加边框并确保在可见区域内
		capture_rect.Expand(BorderSize);
		capture_rect.ClipWithFull(VisibleRect);

		if (!CaptureActive)
			CaptureRect = capture_rect;	// 更新截图区域

		// 显示截图区域尺寸信息
		char buf[128];
		sprintf_s(buf, "%.0f x %.0f", CaptureRect.GetWidth(), capture_rect.GetHeight());
		//g.drawq.AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(1, 1), IM_COL32_WHITE, buf);
		//g.OverlayDrawList.AddRect(CaptureRect.Min - ImVec2(1, 1), CaptureRect.Max + ImVec2(1, 1), IM_COL32_WHITE);

		// 检查是否触发截图(按钮点击或快捷键ALT+C)
		capture |= io.KeyAlt && ImGui::IsKeyPressed(ImGuiKey_C);
		if (capture)
			CaptureStart(windows_selected.empty() ? NULL : windows_selected[0]);	// 开始截图过程

		if (CaptureActive)
			CaptureUpdate();	// 更新截图过程

		ImGui::End();	// 结束ImGui窗口
	}
};