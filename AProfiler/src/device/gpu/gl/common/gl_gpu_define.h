#pragma once
#include <glad/glad.h>
namespace APROFILER {

	namespace GL {
		enum class ElementType
		{
			TYPE_INT = 0,
			TYPE_FLOAT,
			TYPE_VECTOR2,
			TYPE_VECTOR3,
			TYPE_VECTOR4,
			TYPE_UBYTE4,
			TYPE_UBYTE4_NORM,
			MAX_VERTEX_ELEMENT_TYPES
		};

		static const GLenum ELEMENTTYPE[] =
		{
			GL_INT,
			GL_FLOAT,
			GL_FLOAT,
			GL_FLOAT,
			GL_FLOAT,
			GL_UNSIGNED_BYTE,
			GL_UNSIGNED_BYTE
		};
		/*
		Used to mark attribute names and usage
		*/
		enum class ElementSemantic : int
		{
			SEM_POSITION = 0,
			SEM_NORMAL,
			SEM_BINORMAL,
			SEM_TANGENT,
			SEM_TEXCOORD,
			SEM_COLOR,
			SEM_BLENDWEIGHTS,
			SEM_BLENDINDICES,
			SEM_OBJECTINDEX,
			SEM_BONE_ID,
			SEM_BONE_WEIGHT,
			SEM_BIGTANGENT,
			MAX_VERTEX_ELEMENT_SEMANTICS
		};




		//define for texture
		enum class Wrap : int
		{
			REPEAT = 0,
			MIRRORED_REPEAT,
			CLAMP_TO_EDGE,
			BORDER
		};
		static const GLenum TEXTURE_WRAPS[] =
		{
			GL_REPEAT,
			GL_MIRRORED_REPEAT,
			GL_CLAMP_TO_EDGE,
			GL_CLAMP_TO_BORDER
		};

		enum class ColorSpace : int
		{
			RGB = 0,
			RGBA,
			RED
		};

		static const GLenum COLOR_SPACE[] = {
			GL_RGB,
			GL_RGBA,
			GL_RED
		};

		enum class TextureFilter : int
		{
			NEARE = 0,
			LINEAR,
			MIPMAP_LINEAR
		};

		static const GLenum TEXTUREFILTERS[] =
		{
			GL_NEAREST,
			GL_LINEAR,
			GL_LINEAR_MIPMAP_LINEAR
		};

		enum class DataType :int
		{
			BYTE = 0,
			UNSIGNED_BYTE,
			SHORT,
			UNSIGNED_SHORT,
			INT,
			UNSIGNED_INT,
			FLOAT
		};

		static const GLenum DATATYPES[] =
		{
			GL_BYTE,
			GL_UNSIGNED_BYTE,
			GL_SHORT,
			GL_UNSIGNED_SHORT,
			GL_INT,
			GL_UNSIGNED_INT,
			GL_FLOAT
		};



		
	}
}