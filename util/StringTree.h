

class StringTreeNode;
class ValueHashTable;



// a searchable tree that indexes pointers by strings
class StringTree {
        
    public:
        
        StringTree();
        
        ~StringTree();
        

        void insert( const char *inString, void *inValue );

        // supply a string to avoid searching the whole tree for inValue
        void remove( const char *inString, void *inValue );
        
        
        int countMatches( const char *inSearch );
        
        // outValues must have space allocated by caller for inNumToGet 
        // pointers
        int getMatches( const char *inSearch, int inNumToSkip, int inNumToGet,
                        void **outValues );
        
        // prints whole treen in Graphviz format
        void print();
        
    protected:
        
        StringTreeNode *mTreeRoot;
        
        ValueHashTable *mHashTable;
    };
