/*

	PROJECT:		project_fuego
	LICENSE:		See LICENSE in the top level directory
	COPYRIGHT:		Copyright we_sux, BlastHack

	project_fuego is available from nowhere

	project_fuego is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	project_fuego is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with project_fuego.  If not, see <http://www.gnu.org/licenses/>.

*/
// this is pretty std stuff
#define snprintf	_snprintf
#define vsnprintf	_vsnprintf
#define isfinite	_finite

// User-defined warnings - MTA
#define __STR2__( x )	#x
#define __STR1__( x )	__STR2__( x )
#define __LOC__			__FILE__ "("__STR1__( __LINE__ ) ") : warning C0000 *m0d Team*: "
