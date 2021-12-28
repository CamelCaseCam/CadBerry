CadBerry/Rendering/Buffer.h
###########################

Imports
=======
* :doc:`/API/CadBerry/Core.h`
* :doc:`/API/CadBerry/Log.h`
* :doc:`/API/cdbpch.h`

enum class ShaderDataType
=========================
The data type of the buffer elements

Contains
--------
* None* Float* Float2* Float3* Float4* Mat3 (3x3 matrix)* Mat4 (4x4 matrix)* Int* Int2* Int3* Int4* Booluint32_t ShaderDataTypeSize(ShaderDataType DataType)====================================================Returns the size (in bytes) of the DataTypeBufferElement struct====================Fields------ShaderDataType ElementType^^^^^^^^^^^^^^^^^^^^^^^^^^The element's typeuint32_t Offset^^^^^^^^^^^^^^^The element's offsetuint32_t Size^^^^^^^^^^^^^The element's size (in bytes)std::string Name^^^^^^^^^^^^^^^^The element's namebool Normalized^^^^^^^^^^^^^^^Is the data normalized?Functions---------uint32_t GetElementCount()^^^^^^^^^^^^^^^^^^^^^^^^^^Returns the number of whatever OpenGL primitive type (float, int, bool) that is used. For example, Float4 returns 4 because it's 4 floatsBufferLayout class==================This class provides iterators, so you can iterate through it like a vectorFunctions---------const std::vector<BufferElement>& GetElements()^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^Returns the elements in the bufferuint32_t GetStride()^^^^^^^^^^^^^^^^^^^^Returns the stride between verticiesvoid CalculateOffsetsAndStride()^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^Calculates each element's offset and calculates the stride between each vertexFields------uint32_t Stride^^^^^^^^^^^^^^^The stride between each vertexstd::vector<BufferElement> Elements^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^The elements of the bufferVertexBuffer class==================Interface for API-specific vertex buffer implementationFunctions---------virtual void Bind()^^^^^^^^^^^^^^^^^^^Binds the vertex buffervirtual void Unbind()^^^^^^^^^^^^^^^^^^^^^Unbinds the vertex buffervirtual void SetLayout(const BufferLayout& Layout)^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^Sets the vertex buffer's layoutstatic VertexBuffer* Create(uint32_t size, float* vertices)^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^Creates an API-specific vertex bufferIndexBuffer class=================Interface for API-specific index buffer implementationFunctions---------virtual void Bind()^^^^^^^^^^^^^^^^^^^Binds the index buffervirtual void Unbind()^^^^^^^^^^^^^^^^^^^^^Unbinds the index buffervirtual uint32_t GetCount()^^^^^^^^^^^^^^^^^^^^^^^^^^^Returns the number of indicesstatic IndexBuffer* Create(uint32_t count, uint32_t* indices)^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^Creates an API-specific index buffer