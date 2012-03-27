#include <cstdarg>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cassert>
#include <cstdlib>

#include <boost/algorithm/string.hpp>

#include <AL/al.h> // for AL_FORMAT_STEREO16
#include <GL/gl.h>

#include "material.hpp"
#include "renderl.hpp"
#include "renderm.hpp"
#include "renderh.hpp"
#include "audiol.hpp"
#include "resources.hpp"
#include "noise.hpp"
#include "util.hpp"
#include "fswatch.hpp"

using namespace std;

struct resource_program_t
{
    int shader_count;
    char shader_filenames[8][256];

    renderl_program_t uploaded_program;
};

#define MAX_PROGRAM_COUNT 100
static resource_program_t programs[MAX_PROGRAM_COUNT];
static int program_count = 0;

struct resource_texture_t
{
    char filename[256];

    renderl_texture_t uploaded_texture;
};

#define MAX_TEXTURE_COUNT 100
static resource_texture_t textures[MAX_TEXTURE_COUNT];
static int texture_count = 0;

struct resource_wave_t
{
    char filename[256];

    audiol_wave_t uploaded_wave;
};

#define MAX_WAVE_COUNT 100
static resource_wave_t waves[MAX_WAVE_COUNT];
static int wave_count = 0;

void resource_init()
{
}

static void get_dirname(const char* path, string& dirname)
{
	char* strp;
	strp = strrchr((char *)path, '/');
	if(strp == 0)
	{
		dirname = "";
	}
	else
	{
		dirname = string ( path, strp - path + 1 );
	}
}

struct v_index
{
    int i[3];
};

/*renderm_mesh_t resource_load_obj_mesh(const char *filename)
{
	ifstream obj_stream(filename, ios::binary);

	if (!obj_stream) 
	{
		cout << "Can't open file " << filename << endl;
        assert(0);
	}


	string type;
	char dummy[256];
	vector<vec3_t<> > vertices;
	vector<vec3_t<> > normals;
	vector<vec2_t<> > texture_coords;
	string mtllib;
	vector<v_index> texture_coord_indices;
	vector<v_index> vertex_indices;
	vector<v_index> normal_indices;
    float max_length = 0.0f;

	while( !obj_stream.eof() )
	{
		obj_stream >> type;
		if(obj_stream.eof()) // No extra f read after end of file, (type did not change, so f tried to read again)
		{
			break;
		}
		else if(type == "#" || type == "usemtl" || type == "s")
		{
			obj_stream.getline(dummy, 256);
		}
		else if(type == "mtllib")
		{
			obj_stream >> mtllib;
			// Add current path
			string dirname;
			get_dirname(filename, dirname);
			mtllib = dirname + mtllib;
		}
		else if(type == "v")
		{
			vec3_t<> v;
			obj_stream >> v;
			vertices.push_back(v);

            float length = v.length();
            if (length > max_length)
            {
                max_length = length;
            }
		}
		else if(type == "vn")
		{
			vec3_t<> v;
			obj_stream >> v;
			normals.push_back(v);
		}
		else if(type == "vt")
		{
			vec2_t<> v;
			obj_stream >> v;
			v.y = 1-v.y;
			texture_coords.push_back(v);
		}
		else if(type == "f")
		{
			v_index face_vertex_index;
			v_index face_normal_index;
			v_index face_texture_coord_index;
			char dummy_char;
			bool found_texture_coords = false;
			for(size_t i = 0; i < 3; ++i)
			{
				obj_stream >> face_vertex_index.i[i] >> dummy_char;
				char c = obj_stream.peek();
				if ( (c >= '0') && (c <= '9') ) // There exists texture coords
				{
					obj_stream >> face_texture_coord_index.i[i];
					found_texture_coords = true;
				}
				obj_stream >> dummy_char >> face_normal_index.i[i];

				face_texture_coord_index.i[i]--;
				face_normal_index.i[i]--;
				face_vertex_index.i[i]--;
			}
			if(found_texture_coords)
			{
				texture_coord_indices.push_back(face_texture_coord_index);
			}
			normal_indices.push_back(face_normal_index);
			vertex_indices.push_back(face_vertex_index);
		}
		else{
			cout << "MESH LOAD, Unknown row prefix: " << type << endl;
		}
	}
	obj_stream.close();

	// Set array sizes
	int texture_array_size = vertex_indices.size() * 3 * 2;
	int array_size = vertex_indices.size() * 3 * 3;

	//Create new arrays
    float *normal_array = NULL;
    float *vertex_array = NULL;
    float *texture_coords_array = NULL;
	bool has_texture_coords = (texture_coords.size() > 0);
    assert(has_texture_coords);
	if(has_texture_coords)
	{
		texture_coords_array = new float[texture_array_size];
	}
	normal_array = new float[array_size];
	vertex_array = new float[array_size];

	// Set array values from the vectors
	for(size_t i = 0; i < vertex_indices.size(); ++i) //There is a lot of Faces (Vertex)
	{
		for(size_t face_vector = 0; face_vector < 3; ++face_vector) //Each Face (Vextex) has three vectors
		{
			for(size_t xyz = 0; xyz < 3; ++xyz) //Each vector has three floats
			{
				// cout << vertex_indices[i].i[face] << " ";
				vertex_array[i*9 + face_vector*3 + xyz] = vertices[vertex_indices[i].i[face_vector]].c[xyz];
				normal_array[i*9 + face_vector*3 + xyz] = normals[normal_indices[i].i[face_vector]].c[xyz];
			}
			if(has_texture_coords)
			{
				for(size_t xy = 0; xy < 2; ++xy) //Each vector has three floats
				{
					// cout << vertex_indices[i].i[face] << " ";
					texture_coords_array[i*6 + face_vector*2 + xy] = texture_coords[texture_coord_indices[i].i[face_vector]].c[xy];
				}
			}
		}
	}

    int index_count = 3 * vertex_indices.size();
    assert(index_count < 65536);
    unsigned short *index_array = new unsigned short[index_count];
    for (int i = 0; i < index_count; i++)
    {
        index_array[i] = i;
    }

    renderm_mesh_t res;
    res.position_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 3, vertex_array, 3 * sizeof(float) * index_count);
    res.normal_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 3, normal_array, 3 * sizeof(float) * index_count);
    res.tangent_buffer.handle = 0;
    res.tex_coord_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 2, texture_coords_array, 2 * sizeof(float) * index_count);
    res.index_buffer = renderl_upload_index_buffer(GL_UNSIGNED_SHORT, index_array, sizeof(unsigned short) * index_count);
    res.index_count = index_count;
    res.primitive_type = GL_TRIANGLES;

    delete[] index_array;
	delete[] normal_array;
	delete[] vertex_array;
	delete[] texture_coords_array;
	
	return res;
}*/

typedef unsigned char stbi_uc;
extern "C" {
extern stbi_uc *stbi_load            (char const *filename,     int *x, int *y, int *comp, int req_comp);
extern void     stbi_image_free      (void *retval_from_stbi_load);
};

static renderl_texture_t upload_texture(const char *filename)
{
    int width, height, n;
    unsigned char *data = stbi_load(filename, &width, &height, &n, 0);
    if (!data)
    {
        printf("Couldn't open '%s'\n", filename);
        assert(0);
    }

    assert(n == 3 || n == 4);

    int bpl = n * width;

    void *flipped_data = malloc(bpl * height);

    unsigned char *d1 = (unsigned char *)data;
    unsigned char *d2 = (unsigned char *)flipped_data;
    for (int y = 0; y < height; y++)
    {
        memcpy(&d2[(height - y - 1) * bpl], &d1[y * bpl], bpl);
    }

    renderl_texture_t res = renderl_upload_texture(width, height, n == 3 ? GL_RGB : GL_RGBA, flipped_data);

    free(flipped_data);
    stbi_image_free(data);

    return res;
}

static void reload_texture_record(resource_texture_t *record)
{
    renderl_delete_texture(record->uploaded_texture);
    record->uploaded_texture = upload_texture(record->filename);
}

static void reload_texture_record_proxy(void *record)
{
    reload_texture_record((resource_texture_t *)record);
}

const renderl_texture_t *resource_upload_texture(const char *filename)
{
    assert(texture_count < MAX_TEXTURE_COUNT);
    assert(strlen(filename) < sizeof(resource_texture_t::filename) - 1);

    resource_texture_t &record = textures[texture_count++];
    strcpy(record.filename, filename);

    fswatch_add_modified_callback(filename, reload_texture_record_proxy, &record);

    record.uploaded_texture = upload_texture(filename);

    return &record.uploaded_texture;
}

typedef struct stb_vorbis stb_vorbis;
typedef struct
{
   unsigned int sample_rate;
   int channels;

   unsigned int setup_memory_required;
   unsigned int setup_temp_memory_required;
   unsigned int temp_memory_required;

   int max_frame_size;
} stb_vorbis_info;
typedef struct
{
   char *alloc_buffer;
   int   alloc_buffer_length_in_bytes;
} stb_vorbis_alloc;

extern "C"
{
extern int stb_vorbis_decode_filename(char *filename, int *channels, short **output);
extern stb_vorbis_info stb_vorbis_get_info(stb_vorbis *f);
extern void stb_vorbis_close(stb_vorbis *f);
extern stb_vorbis * stb_vorbis_open_filename(char *filename,
                                  int *error, stb_vorbis_alloc *alloc_buffer);
}
static audiol_wave_t upload_wave(const char *filename)
{
    int error;
	stb_vorbis *f = stb_vorbis_open_filename((char *)filename, &error, NULL);
    if (!f)
    {
        printf("Couldn't open '%s'\n", filename);
        assert(0);
    }
    stb_vorbis_info info = stb_vorbis_get_info(f);
    stb_vorbis_close(f);

    int channel_count;
    short *output;
    int sample_count = stb_vorbis_decode_filename((char *)filename, &channel_count, &output);

    if (sample_count == 0)
    {
        printf("Error loading '%s'\n", filename);
        assert(0);
    }
    //printf("Loaded wave %s, sample rate: %d, channel count: %d, sample count: %d\n", filename, info.sample_rate, channel_count, sample_count);

    assert(info.sample_rate == 44100);
    assert(channel_count == 2);

    // down-mix
    short *data = new short[sample_count];
    for (int i = 0; i < sample_count; i++)
    {
        data[i] = (output[i * 2 + 0] + output[i * 2 + 1]) / 2;
    }

    //audiol_wave_t res = audiol_upload_wave(sample_count, channel_count, AL_FORMAT_STEREO16, output);
    audiol_wave_t res = audiol_upload_wave(sample_count, 1, AL_FORMAT_MONO16, data);

    delete[] data;
    delete[] output;

    return res;
}

static void reload_wave_record(resource_wave_t *record)
{
    audiol_delete_wave(record->uploaded_wave);
    record->uploaded_wave = upload_wave(record->filename);
}

static void reload_wave_record_proxy(void *record)
{
    reload_wave_record((resource_wave_t *)record);
}

const audiol_wave_t *resource_upload_wave(const char *filename)
{
    assert(wave_count < MAX_WAVE_COUNT);
    assert(strlen(filename) < sizeof(resource_wave_t::filename) - 1);

    resource_wave_t &record = waves[texture_count++];
    strcpy(record.filename, filename);

    fswatch_add_modified_callback(filename, reload_wave_record_proxy, &record);

    record.uploaded_wave = upload_wave(filename);

    return &record.uploaded_wave;
}

static float recur_noise(int n, vec2_t<> p)
{
    if (n == 1)
    {
        return noise(p);
    }
    else
    {
        return noise(p) + 0.5f * recur_noise(n - 1, 2.0f * p);
    }
}

renderl_texture_t resource_upload_noise_texture(int width, int height)
{
    unsigned char *data = (unsigned char *)malloc(width*height*3);
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int index = 3 * (x + y * width);
            float ix = x / ((float)width);// / (float)width;
            float iy = y / ((float)height);// / (float)height;
            //float n = 255.0f * 0.5f * (1.0f + noise(ix, iy));
            float n = 255.0f * (0.5f + 0.25f * (recur_noise(18, vec2_t<>(ix, iy))));
            data[index + 0] = n;
            data[index + 1] = n;
            data[index + 2] = n;
        }
    }
    renderl_texture_t res = renderl_upload_texture(width, height, GL_RGB, data);
    free(data);
    return res;
}

struct mesh_in_progress_t
{
	vector<vec3_t<> > positions;
	vector<vec3_t<> > normals;
	vector<vec2_t<> > texture_coords;
	vector<int> position_indices;
	vector<int> normal_indices;
	vector<int> texcoord_indices;
};

struct wavefront_material_t
{
    string name;
    float Ns;
    float Ni;
    float d;
    float Tr;
    vec3_t<> Tf;
    int illum;
    vec3_t<> Ka;
    vec3_t<> Kd;
    vec3_t<> Ks;
    vec3_t<> Ke;
    string map_Ka;
    string map_Kd;
    string map_d;
    string map_bump;

    wavefront_material_t() :
        Ns(0.0f),
        Ni(0.0f),
        d(0.0f),
        Tr(0.0f),
        Tf(0.0f, 0.0f, 0.0f),
        illum(2),
        Ka(0.0f, 0.0f, 0.0f),
        Kd(0.0f, 0.0f, 0.0f),
        Ks(0.0f, 0.0f, 0.0f),
        Ke(0.0f, 0.0f, 0.0f)
    {}
};

static renderm_mesh_t *create_mesh(const mesh_in_progress_t &m)
{
    int index_count = m.position_indices.size();
    assert(index_count < 65536);

    float *position_array = new float[3 * index_count];
    float *normal_array = new float[3 * index_count];
    float *tangent_array = new float[3 * index_count];
    float *texture_coord_array = new float[2 * index_count];
    unsigned short *index_array = new unsigned short[index_count];

    float max_length = 0.0f;

    for (int i = 0; i < index_count / 3; i++)
    {
        struct
        {
            vec3_t<> position;
            vec3_t<> normal;
            vec3_t<> tangent;
            vec2_t<> tex_coord;
        } vertices[3];

        // load data for vertices
        for (int j = 0; j < 3; j++)
        {
            vertices[j].position = m.positions[m.position_indices[i * 3 + j]];
            vertices[j].normal = m.normals[m.normal_indices[i * 3 + j]];
            vertices[j].tex_coord = m.texture_coords[m.texcoord_indices[i * 3 + j]];
        }

        // stuff below stolen from http://www.terathon.com/code/tangent.html
        vec3_t<> q0 = vertices[1].position - vertices[0].position;
        vec3_t<> q1 = vertices[2].position - vertices[0].position;
        vec2_t<> tc0 = vertices[1].tex_coord - vertices[0].tex_coord;
        vec2_t<> tc1 = vertices[2].tex_coord - vertices[0].tex_coord;

        vec3_t<> almost_bitangent(-tc1.x*q0.x+tc0.x*q1.x, -tc1.x*q0.y+tc0.x*q1.y, -tc1.x*q0.z+tc0.x*q1.z);
        vec3_t<> bitangent = ((1.0f / (tc0.x * tc1.y - tc1.x * tc0.y)) * almost_bitangent).normalized();

        for (int j = 0; j < 3; j++)
        {
            vertices[j].tangent = bitangent.cross(vertices[j].normal).normalized();
        }

        // store triangle data in arrays
        for (int j = 0; j < 3; j++)
        {
            position_array[i * 3 * 3 + j * 3 + 0] = vertices[j].position.c[0];
            position_array[i * 3 * 3 + j * 3 + 1] = vertices[j].position.c[1];
            position_array[i * 3 * 3 + j * 3 + 2] = vertices[j].position.c[2];
            normal_array[i * 3 * 3 + j * 3 + 0] = vertices[j].normal.c[0];
            normal_array[i * 3 * 3 + j * 3 + 1] = vertices[j].normal.c[1];
            normal_array[i * 3 * 3 + j * 3 + 2] = vertices[j].normal.c[2];
            tangent_array[i * 3 * 3 + j * 3 + 0] = vertices[j].tangent.c[0];
            tangent_array[i * 3 * 3 + j * 3 + 1] = vertices[j].tangent.c[1];
            tangent_array[i * 3 * 3 + j * 3 + 2] = vertices[j].tangent.c[2];
            texture_coord_array[i * 2 * 3 + j * 2 + 0] = vertices[j].tex_coord.c[0];
            texture_coord_array[i * 2 * 3 + j * 2 + 1] = vertices[j].tex_coord.c[1];

            index_array[i * 3 + j] = i * 3 + j;
        }
    }

    renderm_mesh_t *res = new renderm_mesh_t;
    res->position_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 3, position_array, 3 * sizeof(float) * index_count);
    res->normal_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 3, normal_array, 3 * sizeof(float) * index_count);
    res->tangent_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 3, tangent_array, 3 * sizeof(float) * index_count);
    res->tex_coord_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 2, texture_coord_array, 2 * sizeof(float) * index_count);
    res->index_buffer = renderl_upload_index_buffer(GL_UNSIGNED_SHORT, index_array, sizeof(unsigned short) * index_count);
    res->index_count = index_count;
    res->primitive_type = GL_TRIANGLES;

    delete[] position_array;
    delete[] normal_array;
    delete[] tangent_array;
    delete[] texture_coord_array;
    delete[] index_array;

    return res;
}

static renderm_material_t *create_material(const wavefront_material_t &m)
{
    bool map_Ka = m.map_Ka.length();
    bool map_Kd = m.map_Kd.length();
    //bool map_Ks = m.map_Ks.length();
    bool map_d = m.map_d.length();
    bool map_bump = m.map_bump.length();

    renderm_material_t *res = NULL;

    if (!map_d)
    {
        simple_material_t *nmm =  new simple_material_t;
        nmm->ambient = vec3_t<>(0.0f, 0.0f, 0.0);
        nmm->diffuse = vec3_t<>(0.0f, 0.0f, 0.0);
        nmm->specular = vec3_t<>(0.0f, 0.0f, 0.0);
        nmm->shininess = 0.0f;
        nmm->ambient_texture = NULL;
        nmm->diffuse_texture = NULL;
        nmm->specular_texture = NULL;
        nmm->normal_texture = NULL;

        if (map_Ka)
        {
            nmm->ambient_texture = resource_upload_texture(m.map_Ka.c_str());
        }
        if (map_Kd)
        {
            nmm->diffuse_texture = resource_upload_texture(m.map_Kd.c_str());
        }
        /*if (map_Ks)
        {
            nmm->specular_texture = resource_upload_texture(m.map_Ks.c_str());
        }*/
        if (map_bump)
        {
            nmm->normal_texture = resource_upload_texture(m.map_bump.c_str());
        }

        res = nmm;
        res->program = resource_upload_program(2, "data/shaders/simple_material.vert", "data/shaders/simple_material.frag");
    }
    else
    {
        cout << "Unsupported combination: " << map_Ka << map_Kd << map_d << map_bump << endl;
    }

    return res;
}

void resource_load_material_library(const char *filename, vector<wavefront_material_t> *materials)
{
	ifstream mtl_stream(filename);
	
	if (!mtl_stream) 
	{
		cout << "Can't open file " << filename << endl;
        assert(0);
	}

	string type;
	char dummy[256];
	
    wavefront_material_t material;
	
	while (!mtl_stream.eof())
	{
		mtl_stream >> type;
		if (mtl_stream.eof()) // No extra f read after end of file, (type did not change, so f tried to read again)
		{
			break;
		}
		else if (type == "#")// || type == "usemtl" || type == "s")
		{
			mtl_stream.getline(dummy, 256);
		}
		else if (type == "newmtl")
		{
            if (material.name.length())
            {
                materials->push_back(material);
                //cout << "finished material " << material.name << endl;
            }
            material = wavefront_material_t();
			mtl_stream >> material.name;
            //cout << "started material " << material.name << endl;
		}
		else if (type == "Ns")
		{
			mtl_stream >> material.Ns;
		}
		else if (type == "Ni")
		{
			mtl_stream >> material.Ni;
		}
		else if (type == "d")
		{
			mtl_stream >> material.d;
		}
		else if (type == "Tr")
		{
			mtl_stream >> material.Tr;
		}
		else if (type == "Tf")
		{
			mtl_stream >> material.Tf.x >> material.Tf.y >> material.Tf.z;
		}
		else if (type == "illum")
		{
			mtl_stream >> material.illum;
		}
		else if (type == "Ka")
		{
			mtl_stream >> material.Ka;
		}
		else if (type == "Kd")
		{
			mtl_stream >> material.Kd;
		}
		else if (type == "Ks")
		{
			mtl_stream >> material.Ks;
		}
		else if (type == "Ke")
		{
			mtl_stream >> material.Ke;
		}
		else if (type == "map_Ka")
		{
			mtl_stream >> material.map_Ka;
		}
		else if (type == "map_Kd")
		{
			mtl_stream >> material.map_Kd;
		}
		else if (type == "map_d")
		{
			mtl_stream >> material.map_d;
		}
		else if (type == "map_bump")
		{
			mtl_stream >> material.map_bump;
		}
		else if (type == "bump")
		{
			mtl_stream >> material.map_bump;
		}
		else
        {
			cout << "MTL loader, Unknown type: " << type << endl;
            assert(0);
		}
	}
	mtl_stream.close();

    if (material.name.length())
    {
        materials->push_back(material);
        //cout << "finished material " << material.name << endl;
    }
}

renderm_material_t *resource_load_mtl_material(const char *filename)
{
    vector<wavefront_material_t> materials;
    resource_load_material_library(filename, &materials);
    assert(materials.size() == 1);
    return create_material(materials[0]);
}

void resource_load_obj_models(const char *filename, vector<renderh_model_t> *models)
{
	ifstream is(filename, ios::binary);

	if (!is) 
	{
		cout << "Can't open file " << filename << endl;
        assert(0);
	}

	string type;
	char dummy[256];
	string mtllib;
    float max_length = 0.0f;

    mesh_in_progress_t mesh_in_progress;

    string processing_group;

    int line_no = 0;

    vector<wavefront_material_t> library;
    const wavefront_material_t *active_material = NULL;

	while (!is.eof())
	{
		is >> type;
        line_no += 1;
        //cout << "#" << line_no << endl;
		if (is.eof()) // need this to abort at the last (empty) line
		{
			break;
		}
		else if (type == "#" || type == "s")
		{
			is.getline(dummy, 256);
		}
		else if (type == "mtllib")
		{
			is >> mtllib;
			// Add current path
			string dirname;
			get_dirname(filename, dirname);
			mtllib = dirname + mtllib;

            resource_load_material_library(mtllib.c_str(), &library);
		}
        else if (type == "usemtl")
        {
            string material_name;
            is >> material_name;
            bool found = false;
            for (vector<wavefront_material_t>::const_iterator iter = library.begin(); iter != library.end(); iter++)
            {
                const wavefront_material_t &m = *iter;
                if (m.name == material_name)
                {
                    found = true;
                    active_material = &m;
                    break;
                }
            }

            if (!found)
            {
                cout << "Couldn't find material '" << material_name << "'." << endl;
                assert(0);
            }
        }
		else if (type == "v")
		{
            float x, y, z;
            is >> x >> y >> z;
			vec3_t<> v(0.2f * x, 0.2f * y, 0.2f * z);
			mesh_in_progress.positions.push_back(v);

            float length = v.length();
            if (length > max_length)
            {
                max_length = length;
            }
		}
		else if (type == "vn")
		{
            float x, y, z;
            is >> x >> y >> z;
			vec3_t<> v(x, y, z);
			mesh_in_progress.normals.push_back(v);
		}
		else if (type == "vt")
		{
            float s, t;//, r;
            is >> s >> t;// >> r;
			vec2_t<> v(s, t);
			v.y = 1-v.y;
			mesh_in_progress.texture_coords.push_back(v);
		}
		else if (type == "f")
		{
			int fposition_indices[3];
			int fnormal_indices[3];
			int ftexcoord_indices[3];
			char dummy_char;
            int next_index = 0;
			bool found_texture_coords = false;

            std::string line;
            getline(is, line);
            std::vector<std::string> strs;
            boost::split(strs, line, boost::is_any_of(" "));

            //cout << line << strs.size() << endl;

            for (vector<string>::const_iterator iter = strs.begin(); iter != strs.end(); iter++)
            {
                const string &s = *iter;
                if (s.length())
                {
                    std::vector<std::string> parts;
                    boost::split(parts, s, boost::is_any_of("/"));
                    if (parts.size() == 3)
                    {
                        fposition_indices[next_index] = atoi(parts[0].c_str()) - 1;
                        ftexcoord_indices[next_index] = atoi(parts[1].c_str()) - 1;
                        fnormal_indices[next_index] = atoi(parts[2].c_str()) - 1;

                        next_index += 1;

                        if (next_index == 3)
                        {
                            // first flush triangle
                            for (int i = 0; i < 3; i++)
                            {
                                mesh_in_progress.position_indices.push_back(fposition_indices[i]);
                                mesh_in_progress.normal_indices.push_back(fnormal_indices[i]);
                                mesh_in_progress.texcoord_indices.push_back(ftexcoord_indices[i]);
                            }

                            // then shuffle indices
                            fposition_indices[1] = fposition_indices[2];
                            fnormal_indices[1] = fnormal_indices[2];
                            ftexcoord_indices[1] = ftexcoord_indices[2];
                            next_index = 2;
                        }
                    }
                }
            }
		}
		else if (type == "g")
        {
            if (processing_group.length())
            {
                cout << "finished working on " << processing_group << endl;

                renderh_model_t model;
                model.mesh_count = 1;
                model.meshes[0] = create_mesh(mesh_in_progress);
                if (active_material == NULL)
                {
                    assert(library.size() == 1);
                    active_material = &library[0];
                }
                model.materials[0] = create_material(*active_material);
                if (model.materials[0])
                {
                    models->push_back(model);
                    static int count = 5000;
                    if (count-- == 0)
                    {
                        return;
                    }
                }
                mesh_in_progress.position_indices.clear();
                mesh_in_progress.normal_indices.clear();
                mesh_in_progress.texcoord_indices.clear();
                active_material = NULL;
            }
            is >> processing_group;

            //cout << "started working on " << processing_group << endl;
        }
		else
        {
			cout << "Unknown type: " << type << " on line " << line_no << endl;
            assert(0);
		}
	}
	is.close();

    if (processing_group.length())
    {
        //cout << "finished working on " << processing_group << endl;
        renderh_model_t model;
        model.mesh_count = 1;
        model.meshes[0] = create_mesh(mesh_in_progress);
        if (active_material == NULL)
        {
            assert(library.size() == 1);
            active_material = &library[0];
        }
        model.materials[0] = create_material(*active_material);
        if (model.materials[0])
        {
            models->push_back(model);
        }
        mesh_in_progress.position_indices.clear();
        mesh_in_progress.normal_indices.clear();
        mesh_in_progress.texcoord_indices.clear();
        active_material = NULL;
    }
}

renderh_model_t resource_load_obj_model(const char *filename)
{
    vector<renderh_model_t> models;
    resource_load_obj_models(filename, &models);
    assert(models.size() == 1);
    return models[0];
}

/*
struct resource_program_t
{
    int shader_count;
    char shader_filenames[8][80];

    renderl_program_t uploaded_program;
};
*/

static renderl_program_t upload_program(int shader_count, char filenames[8][256])
{
    renderl_source_t shader_sources[16];
    int shader_source_count = 0;

    for (int i = 0; i < shader_count; i++)
    {
        const char *filename = filenames[i];

        char buffer[1024 * 16];
        int read = slurp(filename, buffer, sizeof(buffer));
        assert(read > 0 && read != sizeof(buffer));

        if (strstr(filename, ".vert") != NULL)
        {
            char *buffer_copy = new char[read];
            memcpy(buffer_copy, buffer, read);
            renderl_source_t &source = shader_sources[shader_source_count++];
            source.type = GL_VERTEX_SHADER;
            source.name = filename;
            source.source = buffer_copy;
            source.source_size = read;
        }
        else if (strstr(filename, ".frag") != NULL)
        {
            char *buffer_copy = new char[read];
            memcpy(buffer_copy, buffer, read);

            renderl_source_t &source = shader_sources[shader_source_count++];
            source.type = GL_FRAGMENT_SHADER;
            source.name = filename;
            source.source = buffer_copy;
            source.source_size = read;
        }
        else
        {
            char *buffer_copy0 = new char[read];
            memcpy(buffer_copy0, buffer, read);
            char *buffer_copy1 = new char[read];
            memcpy(buffer_copy1, buffer, read);

            renderl_source_t &source0 = shader_sources[shader_source_count++];
            source0.type = GL_FRAGMENT_SHADER;
            source0.name = filename;
            source0.source = buffer_copy0;
            source0.source_size = read;
            renderl_source_t &source1 = shader_sources[shader_source_count++];
            source1.type = GL_VERTEX_SHADER;
            source1.name = filename;
            source1.source = buffer_copy1;
            source1.source_size = read;
        }
    }

    renderl_program_t res = renderl_upload_program(shader_source_count, shader_sources);

    // clean up
    for (int i = 0; i < shader_source_count; i++)
    {
        renderl_source_t &source = shader_sources[i];
        delete[] source.source;
    }

    return res;
}

static void reload_program_record(resource_program_t *record)
{
    renderl_program_t res = upload_program(record->shader_count, record->shader_filenames);
    if (res.handle != 0)
    {
        renderl_delete_program(record->uploaded_program);
        record->uploaded_program = res;
    }
}

static void reload_program_record_proxy(void *record)
{
    reload_program_record((resource_program_t *)record);
}

void resource_reload_programs()
{
    for (int i = 0; i < program_count; i++)
    {
        reload_program_record(&programs[i]);
    }
}

const renderl_program_t *resource_upload_program(int shader_count, ...)
{
    assert(shader_count < 8);

    assert(program_count < MAX_PROGRAM_COUNT);
    resource_program_t &record = programs[program_count++];

    record.shader_count = shader_count;

    va_list argp;
    va_start(argp, shader_count);

    for (int i = 0; i < shader_count; i++)
    {
        const char *filename = va_arg(argp, const char *);
        assert(strlen(filename) < sizeof(resource_program_t::shader_filenames[0]) - 1);

        strcpy(record.shader_filenames[i], filename);

        fswatch_add_modified_callback(filename, reload_program_record_proxy, &record);
    }

    va_end(argp);

    record.uploaded_program = upload_program(record.shader_count, record.shader_filenames);
    assert(record.uploaded_program.handle != 0);

    return &record.uploaded_program;
}

