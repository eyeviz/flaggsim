#ifndef __OZ_OPTION_CONFIG_H__
#define __OZ_OPTION_CONFIG_H__

#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace::std;

namespace OZ {
	namespace Option {
		class Config {

			private:
				map< string, string > m_map;

			public:
				Config( const string& filename = "" ) {
					LoadConfigFile( filename );
				}
				~Config(){}

				double getlf( const string& key ) const {
					map< string, string >::const_iterator it = m_map.find( key );

					if( it == m_map.end() )
					{
						cerr << "[ERR] " << key << " is not a defined option." << endl;
						return 0.0;
					}

					return atof( it->second.c_str() );
				}

				float getf( const string& key ) const
				{
					map< string, string >::const_iterator it = m_map.find( key );

					if( it == m_map.end() )
					{
						cerr << "[ERR] " << key << " is not a defined option." << endl;
						return 0.0f;
					}

					return atof( it->second.c_str() );
				}

				int geti( const string& key ) const
				{
					map< string, string >::const_iterator it = m_map.find(key);

					if( it == m_map.end() )
					{
						cerr << "[ERR] " << key << " is not a defined option." << endl;
						return 0;
					}

					return atoi( it->second.c_str() );
				}

				string gets( const string& key ) const
				{
					map< string, string>::const_iterator it = m_map.find( key );

					if( it == m_map.end() )
					{
						cerr << "[ERR] " << key << " is not a defined option." << endl;
						return string( "" );
					}

					return it->second;
				}

				bool has( const string& key ) const
				{
					if( m_map.find( key ) == m_map.end() )
					{
						return false;
					}

					return true;
				}

			private:

				void split( const string& in, std::vector< string >& out )
				{
					out.clear();
					out.resize(2);

					string::size_type sp_st  = in.find( " ", 0 );
					string::size_type sp_ed  = sp_st;

					if( sp_st == string::npos ) return;

					while( true ){
						if( in.at( ++sp_ed ) != ' ' )
						{
							break;
						}
					}

					out[0] = in.substr( 1, sp_st - 1 );
					out[1] = in.substr( sp_ed, in.length() );

				}

				void LoadConfigFile( const string& filename )
				{
					ifstream ifs( filename.c_str(), ios::in );
					if( ifs.fail() )
					{
						cerr << "[ERR] file open error " << filename << endl;
						return;
					}

					string buf;

					while( getline( ifs, buf ) )
					{
						if( buf.length() == 0   ) continue;
						if( buf.at(0)    != ':' ) continue;

						std::vector< string > vbuf;
						split( buf, vbuf );
#ifdef  DEBUG
						cerr << "buf = " << buf << endl;
						for( int i = 0; i < vbuf.size(); i++ ){
							cerr << "vbuf[" << i << "] = " << vbuf[i] << endl;
						}
#endif  // DEBUG
						m_map.insert( make_pair(vbuf[0], vbuf[1]) );
					}
				}

			};

	} // namespace Option

} // namespace OZ


#endif // __OZ_OPTION_CONFIG_H__
