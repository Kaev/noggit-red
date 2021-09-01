# Class: chunk

Represents a chunk in the world

## Table of contents

### Constructors

- [constructor](chunk.md#constructor)

### Methods

- [add\_texture](chunk.md#add_texture)
- [apply](chunk.md#apply)
- [apply\_all](chunk.md#apply_all)
- [apply\_heightmap](chunk.md#apply_heightmap)
- [apply\_textures](chunk.md#apply_textures)
- [apply\_vertex\_color](chunk.md#apply_vertex_color)
- [clear\_colors](chunk.md#clear_colors)
- [clear\_textures](chunk.md#clear_textures)
- [get\_area\_id](chunk.md#get_area_id)
- [get\_effect](chunk.md#get_effect)
- [get\_tex](chunk.md#get_tex)
- [get\_texture](chunk.md#get_texture)
- [get\_texture\_count](chunk.md#get_texture_count)
- [get\_vert](chunk.md#get_vert)
- [remove\_texture](chunk.md#remove_texture)
- [set\_area\_id](chunk.md#set_area_id)
- [set\_effect](chunk.md#set_effect)
- [set\_hole](chunk.md#set_hole)
- [set\_impassable](chunk.md#set_impassable)
- [to\_selection](chunk.md#to_selection)

## Constructors

### constructor

\+ **new chunk**(): [*chunk*](chunk.md)

**Returns:** [*chunk*](chunk.md)

## Methods

### add\_texture

▸ **add_texture**(`texture`: *string*, `effect`: *number*): *number*

Adds a new texture at the current topmost layer.

**`note`** A chunk can hold at most 4 texture layers.

#### Parameters:

Name | Type | Description |
:------ | :------ | :------ |
`texture` | *string* |  |
`effect` | *number* | effect id to add -                -2 (default): does not change effect -                -1: clears current effect index -                0+: change to this effect index   |

**Returns:** *number*

texture index added to

___

### apply

▸ **apply**(): *void*

Same as apply_all

**Returns:** *void*

___

### apply\_all

▸ **apply_all**(): *void*

Applies all changes in this chunk

**Returns:** *void*

___

### apply\_heightmap

▸ **apply_heightmap**(): *void*

Applies all changes to the heightmap in this chunk

**Returns:** *void*

___

### apply\_textures

▸ **apply_textures**(): *void*

Applies all changes to texture alphamaps in this chunk

**Returns:** *void*

___

### apply\_vertex\_color

▸ **apply_vertex_color**(): *void*

Applies all changes to vertex colors in this chunk

**Returns:** *void*

___

### clear\_colors

▸ **clear_colors**(): *void*

Removes all vertex colors in this chunk

**Returns:** *void*

___

### clear\_textures

▸ **clear_textures**(): *void*

Removes all texture layers in this chunk

**Returns:** *void*

___

### get\_area\_id

▸ **get_area_id**(): *number*

Returns the area id of a chunk

**Returns:** *number*

___

### get\_effect

▸ **get_effect**(`layer`: *number*): *number*

Returns the effect id at a texture layer

#### Parameters:

Name | Type |
:------ | :------ |
`layer` | *number* |

**Returns:** *number*

___

### get\_tex

▸ **get_tex**(`index`: *number*): [*tex*](tex.md)

Returns a texel by index in this chunk

#### Parameters:

Name | Type | Description |
:------ | :------ | :------ |
`index` | *number* | valid in range [0-4095]    |

**Returns:** [*tex*](tex.md)

___

### get\_texture

▸ **get_texture**(`index`: *number*): *string*

Returns the name of the texture at the specified layer.

#### Parameters:

Name | Type |
:------ | :------ |
`index` | *number* |

**Returns:** *string*

___

### get\_texture\_count

▸ **get_texture_count**(): *number*

Returns the amount of textures on this chunk

**Returns:** *number*

___

### get\_vert

▸ **get_vert**(`index`: *number*): [*vert*](vert.md)

Returns a vertex by index in this chunk

#### Parameters:

Name | Type | Description |
:------ | :------ | :------ |
`index` | *number* | valid in range [0-144]    |

**Returns:** [*vert*](vert.md)

___

### remove\_texture

▸ **remove_texture**(`index`: *number*): *void*

Removes a texture layer from this chunk
and decreases the texture ids of all higher layers by 1

#### Parameters:

Name | Type |
:------ | :------ |
`index` | *number* |

**Returns:** *void*

___

### set\_area\_id

▸ **set_area_id**(`value`: *number*): *void*

Changes the area id of a chunk

#### Parameters:

Name | Type |
:------ | :------ |
`value` | *number* |

**Returns:** *void*

___

### set\_effect

▸ **set_effect**(`layer`: *number*, `effect`: *number*): *any*

Changes the effect id at a texture layer

#### Parameters:

Name | Type | Description |
:------ | :------ | :------ |
`layer` | *number* |  |
`effect` | *number* | effect id to set (-1 to remove effects)    |

**Returns:** *any*

___

### set\_hole

▸ **set_hole**(`hole`: *boolean*): *void*

Creates or removes a hole in this chunk

#### Parameters:

Name | Type |
:------ | :------ |
`hole` | *boolean* |

**Returns:** *void*

___

### set\_impassable

▸ **set_impassable**(`impassable`: *boolean*): *void*

Sets whether this chunk should be impassable for players or not

#### Parameters:

Name | Type |
:------ | :------ |
`impassable` | *boolean* |

**Returns:** *void*

___

### to\_selection

▸ **to_selection**(): [*selection*](selection.md)

Returns a selection spanning this chunk

**`note`** - iterating will include border vert/texels

**Returns:** [*selection*](selection.md)
