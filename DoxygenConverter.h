//
//  DoxygenConverter.h
//  appledoc
//
//  Created by Tomaz Kragelj on 11.4.09.
//  Copyright 2009 Tomaz Kragelj. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Constants.h"

@class CommandLineParser;

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
/** The doxygen output converter class.

This class handles the doxygen xml output files and converts them to DocSet. The
conversion happens through several steps:
  - If @c Doxyfile doesn't exist or doxygen configuration file is not passed over via
	the command line parameters, the default file is created using the doxygen itself,
	then the configuration file options are set so that correct output is used.
  - Doxygen is started with the configuration file which results in xml files being
	created at the desired output path.
  - The generated xml files are parsed and converted to clean versions which are used
	for creating the rest of the documentation. All index xml files are created as well.
  - All references in the cleaned xml files are checked so that they point to the
	correct files and members.
  - Optionally, all cleaned xml files are converted to xhtml.
  - Optionally, the DocSet bundle is created.
  - All temporary files are optionally removed.
 
The convertion takes several steps. In the first steps the objects database is generated
which is used in later steps to get and handle the list of documented objects. The database
is a standard @c NSDictionary of the following layout:
  - @c "Index" key: contains a @c NSXMLDocument with clean index XML.
  - @c "Hierarchy" key: contains a @c NSXMLDocument with clean hierarchy XML.
  - @c "Hierarchies" key: contains a @c NSMutableDictionary with classes hierarchy tree.
	  - @c "<ObjectName>" key: contains a @c NSMutableDictionary describing the object:
		  - @c "ObjectName" key: contains the object name. This is the same name as used
			for the key in the parent dictionary. It still serves a purpose for the
			non-documented objects which are part of the hierarchy - for there we can't
			use the @c "ObjectData" key since we have no entry...
		  - @c "ObjectData" key: contains a pointer to the object's data under the main
			@c "Objects" key list. Note that this is @c nil if the object is not
			documented.
		  - @c "Children" key: contains a @c NSMutableDictionary with all children of
			this object. If the object doesn't have any children, empty dictionary is
			used. The dictionary has the same structure as the main @c "Hierarchies"
			dictionary:
			  - @c "<ObjectName>"...
			  - ...
	  - @c "<ObjectName>"...
	  - ...
  - @c "Objects" key: contains a @c NSMutableDictionary with object descriptions. This
	is usefull for enumerating over all documented objects:
	  - @c "<ObjectName>" key: contains a @c NSMutableDictionary with object data:
		  - @c "ObjectName" key: an @c NSString with the object name (this is the same
			name as used for the key in the root dictionary).
		  - @c "ObjectKind" key: an @c NSString which has the value @c "class" if the
			object is a class, @c "category" if the object is category and @c "protocol"
			if the object is a protocol.
		  - @c "ObjectClass key: an @c NSString which contains the name of the class to
			which the object "belongs". At the moment this is only used for categories
			to map to the class that the category extends. This is key is missing for 
			other objects. Note that the key is also missing for categories which
			"parent" class cannot be determined (not likely, but be prepared just in
			case).
		  - @c "CleanedMarkup" key: contains an @c NSXMLDocument with clean XML. This
			document is updated through different steps and always contains the last
			object data.
		  - @c "Members" key: contains a @c NSMutableDictionary with the descriptions
			of all object members. This is mainly used for nicer links generation. The
			keys for the dictionary are simply method names:
			  - @c "<MethodName>" key: contains a @c NSMutableDictionary with member
				description:
				  - @c "Name" key: a @c NSString with the member name (this is the same
					name as the key in the parent dictionary).
				  - @c "Prefix" key: a @c NSString with the prefix to be used before the
					name in order to get the selector name.
				  - @c "Selector" key: a @c NSString with the correctly formatted member
					selector that can be used directly when creating member link names
					within the same object (inter object links cannot use this because
					their template might include prefix at arbitrary place).
			  - @c "<MethodName>"...
			  - ...
		  - @c "Parent" key: A @c NSString containing the name of the object parent.
			This is only used for classes and is left out for other objects.
		  - @c "RelativeDirectory" key: this @c NSString describes the sub directory 
			under which the object will be stored relative to the index file. At the
			moment this value depends on the object type and can be @c "Classes", 
			@c "Categories" or @c "Protocols".
		  - @c "RelativePath" key: this @c NSString describes the relative path including
			the file name to the index file. This path starts with the value of the
			@c RelativeDirectory key to which the object file name is added.
		  - @c "DoxygenMarkupFilename" key: contains an @c NSString that specifies the
			original name of the XML generated by the doxygen.
	  - @c "<ObjectName>"...
	  - ...
  - @c "Directories" key: contains a @c NSMutableDictionary which resembles the file
	structure under which the objects are stored. This is usefull for enumerating over
	the documented objects by their relative directory under which they will be saved:
	  - @c "<DirectoryName>" key: contains a @c NSMutableArray with the list of all
		objects for this directory. The objects stored in the array are simply pointers
		to the main @c "Objects" instances.
	  - @c "<DirectoryName>"...
	  - ...
 
Note that this class relies on @c CommandLineParser to determine the exact conversion
work flow and common, application-wide parameters. Internally the class delegates
all output generation to top level @c OutputProcessing conformers which in turn manage
all their dependent generators.

This class doesn't perform any actual output generation. Instead it delegates it to the
concrete @c OutputGenerator instances.
*/
@interface DoxygenConverter : NSObject 
{
	CommandLineParser* cmd;
	NSMutableDictionary* database;
	NSMutableArray* topLevelGenerators;
}

//////////////////////////////////////////////////////////////////////////////////////////
/// @name Converting handling
//////////////////////////////////////////////////////////////////////////////////////////

/** Converts the doxygen generated file into the desired output.
 
@exception NSException Thrown if conversion fails.
*/
- (void) convert;

@end
