//
// Created by Yggdrasils on 2019/5/27.
//

#include "utility.hpp"
#include <cstring>
#include <functional>
#include <fstream>
#include <cstddef>
#include <iostream>
#include "exception.hpp"

#define MAXN 4096
#define FileLength 12

namespace sjtu {

    template <class Key, class Value, class Compare = std::less<Key> >

    class BTree {
        typedef off_t offset_t;

    public:
        typedef pair<const Key, Value> value_type;
        static const int M = ((sizeof(Key) + sizeof(offset_t)) > 4096)? 1:4096/(sizeof(Key) + sizeof(offset_t));
        static const int L = (sizeof(value_type)*2 > 4096) ?1:4096/(sizeof(value_type)*2);
        struct dataNodeLeaf                 //叶子结点数据
        {
            Key k;
            Value value;                    //数据
        };

        struct leafNode                     //叶子结点
        {
            short dataSize;                 //数据数量
            /*bool isdelete;                  //块是否从文件中删除*/
            offset_t nodeOffset;
            offset_t next;
            offset_t pre;
            offset_t parent;
            dataNodeLeaf data[L];        //数据

            leafNode():dataSize(0),nodeOffset(0),next(0),pre(0),parent(0){}
        };

        struct dataNodeInter                //内部结点数据
        {
            Key k;
            offset_t dataPos;               //文件偏移量
        };

        struct interNode                    //内部结点
        {
            short dataSize;
            /*bool isdelete;                  //块是否从文件中删除*/
            offset_t offset;
            offset_t parent;
            bool isLeaf;                    //孩子是否为叶子结点
            dataNodeInter data[M];       //pair数

            interNode():offset(0),parent(0),isLeaf(false),dataSize(1){}
        };

        struct fileInfor
        {
            offset_t headLeaf;
            offset_t tailLeaf;
            offset_t End;
            offset_t root;
            size_t size;
            fileInfor():headLeaf(0),tailLeaf(0),End(0),root(0),size(0){}
        };

    private:
        FILE *pFile;
        interNode *root;            //根结点
        offset_t rootOffSet;        //根结点的位置
        offset_t headLeaf;
        offset_t tailLeaf;
        offset_t End;
        size_t sizeLeaf;
        char FileName[FileLength];
        bool isExist;
        bool isOpen;
        char rwBuffer[MAXN];

        /** 本来使用fstream读写文件
         * 结果bug太多，操作不了
         * 所以学习了方指导的例子
         * 使用C风格读写文件
         */

        inline void openFile() {
            isExist = 1;
            if (isOpen == 0) {
                pFile = fopen(FileName, "rb+");
                if (pFile == nullptr) {
                    isExist = 0;
                    pFile = fopen(FileName, "w");
                    fclose(pFile);
                    pFile = fopen(FileName, "rb+");
                }
                //else readFile(&info, info_offset, 1, sizeof(basicInfo));
                isOpen = 1;
            }

        }

        inline void close_File() {
            if (isOpen == 1) {
                fclose(pFile);
                isOpen = 0;
            }
        }


        inline void readIndex(interNode *node, offset_t offset) const {

            if (fseek(pFile, offset, SEEK_SET)) throw "open file failed!";
            //将pFile中的内容读入place
            fread(node, sizeof(interNode), 1, pFile);

        }

        inline void readLeaf(leafNode *leaf, offset_t offset) const {
            if (fseek(pFile,offset,SEEK_SET)) throw "open file failed!";
            fread(leaf, sizeof(leafNode),1,pFile);
        }


        inline void writeIndex(interNode *node, offset_t offset) const {
            if (fseek(pFile, offset, SEEK_SET)) throw "open file failed!";
            //将place中的内容写入pFile
            fwrite(node, sizeof(interNode), 1, pFile);
        }

        inline void writeLeaf(leafNode *leaf, offset_t offset) const {
            if (fseek(pFile, offset, SEEK_SET)) throw "open file failed!";
            //
            fwrite(leaf, sizeof(leafNode),1,pFile);
        }
        /**readLeaf():
         *      从文件中读入叶子结点
         *      给定偏移量offset
         *      返回是否成功读入
         */
        /*inline void readLeaf(leafNode *p,offset_t offset) {
            if (!pFile.is_open()) pFile.open(FileName,std::ios::in | std::ios::out);
            pFile.seekg(offset,std::ios::cur);
            if (!pFile.fail()) {
                pFile.close();
                throw "open file failed";
            }
            pFile.read(rwBuffer,MAXN);
            memcpy(p,rwBuffer, sizeof(leafNode));
        }*/

        /**readInter():
         *      从文件中读入内部结点
         *      给定偏移量offset
         *      返回是否成功读入
         */
        /*inline void readInter(interNode *p, offset_t offset) {
            if (!pFile.is_open()) pFile.open(FileName,std::ios::in | std::ios::out);
            pFile.seekg(offset,std::ios::cur);
            if (!pFile.fail()) {
                pFile.close();
                throw "open file failed";
            }
            pFile.read(rwBuffer,MAXN);
            memcpy(p,rwBuffer, sizeof(interNode));
        }*/

        /**writeLeaf():
         *      将叶子结点写入文件末尾
         *      返回是否成功写入
         */
        /*inline void writeleaf(leafNode *p,offset_t offset) {
            if (!pFile.is_open()) pFile.open(FileName,std::ios::in | std::ios::out);
            if (offset == 0) {
                pFile.seekg(0, std::ios_base::end);
                offset = pFile.tellg();
            }
            p->nodeOffset = offset;
            if (pFile.fail()) {
                pFile.close();
                throw "open file failed";
            }
            pFile.seekp(offset);
            memcpy(rwBuffer,p, sizeof(leafNode));
            pFile.write(rwBuffer, sizeof(char)*MAXN);
            pFile.flush();
        }*/

        /**writeInter():
        *      将内部结点写入文件末尾
        *      返回是否成功写入
        */
        /*inline void writeInter(interNode *p,offset_t offset) {
            if (!pFile.is_open()) pFile.open(FileName,std::ios::in | std::ios::out);
            if (offset == 0) {
                pFile.seekg(0, std::ios_base::end);
                offset = pFile.tellg();
            }
            p->offset = offset;
            if (pFile.fail()) {
                pFile.close();
                throw "open file failed";
            }
            pFile.seekp(offset);
            memcpy(rwBuffer,p, sizeof(interNode));
            pFile.write(rwBuffer, sizeof(char)*MAXN);
            pFile.flush();
        }*/
//===========================================file operation======================================//

    public:




        class const_iterator;

        class iterator {

        private:

            bool isLeaf;
            offset_t offset;


        public:

            bool modify(const Value& value_t){

            }

            iterator() {
                isLeaf = false;
                offset = 0;
            }

            iterator(const iterator& other) {
                isLeaf = other.isLeaf;
                offset = other.offset;
            }


            // Return a new iterator which points to the n-next elements

            iterator operator++(int) {
                // Todo iterator++
            }

            iterator& operator++() {
                // Todo ++iterator

            }

            iterator operator--(int) {
                // Todo iterator--
            }

            iterator& operator--() {
                // Todo --iterator
            }

            // Overloaded of operator '==' and '!='

            // Check whether the iterators are same

            bool operator==(const iterator& rhs) const {
                // Todo operator ==

            }

            bool operator==(const const_iterator& rhs) const {
                // Todo operator ==

            }

            bool operator!=(const iterator& rhs) const {
                // Todo operator !=

            }

            bool operator!=(const const_iterator& rhs) const {
                // Todo operator !=

            }

        };

        class const_iterator {

            // it should has similar member method as iterator.

            //  and it should be able to construct from an iterator.

        private:

        public:

            const_iterator() {

            }

            const_iterator(const const_iterator& other) {

            }

            const_iterator(const iterator& other) {

            }

            const_iterator &operator++(int){

            }
            // And other methods in iterator, please fill by yourself.

        };

        // Default Constructor and Copy Constructor

        BTree() {
            headLeaf = tailLeaf = End = rootOffSet = sizeLeaf = 0;
            strcpy(FileName,"target.txt");
            openFile();
            if (!isExist) BuildTree();
        }



        BTree(const BTree& other) {
            pFile = other.pFile;
            root = other.root;
            rootOffSet = other.rootOffSet;
            headLeaf = other.headLeaf;
            tailLeaf = other.tailLeaf;
            End = other.End;
            sizeLeaf = other.sizeLeaf;
            strcpy(FileName,other.FileName);
            isExist = other.isExist;
        }

        BTree& operator=(const BTree& other) {
            if (other == *this ) {
                return *this;
            }
            pFile = other.pFile;
            root = other.root;
            rootOffSet = other.rootOffSet;
            headLeaf = other.headLeaf;
            tailLeaf = other.tailLeaf;
            End = other.End;
            sizeLeaf = other.sizeLeaf;
            strcpy(FileName,other.FileName);
            isExist = other.isExist;
            return *this;
        }

        ~BTree() {
            close_File();

        }

        //This is right
        pair<iterator,OperationResult> insert_leafNode(const Key &key, const Value &value, leafNode &leaf) {
            iterator ret;
            int l = 0, r = leaf.dataSize, mid = (l + r)/2;
            while(l < r) {
                if (key == leaf.data[mid].k) return pair<iterator,OperationResult> (ret, Fail);
                if (key < leaf.data[mid].k) r = mid;
                else l = mid + 1;
                mid = (l + r) / 2;
            }
            int cnt = mid;

            for (int i = leaf.dataSize-1; i > cnt; --i) {
                leaf.data[i+1].k = leaf.data[i].k;
                leaf.data[i+1].value = leaf.data[i].value;
            }
            leaf.data[cnt+1].k = key; leaf.data[cnt+1].value = value;
            leaf.dataSize++;
            if (leaf.dataSize > L) split_node(&leaf, key);
            else writeLeaf(&leaf, leaf.nodeOffset);
            return pair<iterator,OperationResult> (ret,Success);
        }

        void split_node(leafNode *leaf, const Key &key) {
            leafNode tmpLeaf;
            tmpLeaf.dataSize = leaf->dataSize - leaf->dataSize/2;
            int pos = leaf->dataSize / 2;
            for (int i = 0; i < tmpLeaf.dataSize; ++i) {
                tmpLeaf.data[i] = leaf->data[i + pos];
            }
            tmpLeaf.nodeOffset = End;
            tmpLeaf.pre = leaf->nodeOffset; tmpLeaf.next = leaf->next;
            if (leaf->next == 0) tailLeaf = tmpLeaf.nodeOffset;          //如果原来的叶子是最后一个叶子结点
            else{
                leafNode endLeaf;           //分裂前的next叶子
                readLeaf(&endLeaf,tmpLeaf.next);
                endLeaf.pre = tmpLeaf.nodeOffset;
                writeLeaf(&endLeaf,endLeaf.nodeOffset);
            }
            End = tmpLeaf.nodeOffset + sizeof(leafNode);

            writeLeaf(&tmpLeaf,tmpLeaf.nodeOffset);
            writeLeaf(leaf, leaf->nodeOffset);
            insert_interNode(key,leaf->parent,leaf->nodeOffset);
        }

        pair<iterator, OperationResult > insert_interNode(const Key &key, offset_t interoffset,offset_t leafoffset) {
            interNode inter;
            readIndex(&inter,interoffset);
            int l = 0, r = inter.dataSize-1, mid = (l + r)/2;
            while(l < r) {
                if (key < inter.data[mid].k) r = mid;
                else l = mid + 1;
                mid = (l + r) / 2;
            }

            int pos = mid;
            for (int i = inter.dataSize-1; i > pos;i--) {
                inter.data[i+1] = inter.data[i];
            }
            inter.data[pos+1].k = key;
            inter.data[pos+1].dataPos = leafoffset;
            inter.dataSize++;
            if (inter.dataSize > M) split_index(&inter,interoffset);
            else writeIndex(&inter,inter.offset);
        }

        void split_index(interNode *index, offset_t offset) {
            interNode tmpNode;
            tmpNode.isLeaf = index->isLeaf;
            tmpNode.dataSize= index->dataSize - index->dataSize/2;
            offset_t pos = index->dataSize / 2;
            for (int i = 0; i < tmpNode.dataSize; ++i) {
                tmpNode.data[i] = index->data[i+pos];
            }
            index->dataSize = index->dataSize /2;

            if (tmpNode.isLeaf) {
                leafNode leaf;
                for (int i=0; i < tmpNode.dataSize; ++i){
                    readLeaf(&leaf,tmpNode.data[i].dataPos);
                    leaf.parent = tmpNode.offset;
                    writeLeaf(&leaf,leaf.nodeOffset);
                }
            }
            else {
                interNode inter;
                for (int i = 0; i < tmpNode.dataSize; ++i) {
                    readIndex(&inter,tmpNode.data[i].dataPos);
                    inter.parent = tmpNode.offset;
                    writeIndex(&inter,inter.offset);
                }
            }
            //更新根节点
            if (tmpNode.offset == rootOffSet) {
                interNode newroot;
                newroot.offset = End;
                End += sizeof(interNode);
                newroot.dataSize = 2;
                newroot.data[0] = index->data[0];
                newroot.data[1] = tmpNode.data[0];
                index->parent = tmpNode.parent = newroot.offset;
                root = &newroot;
                rootOffSet = newroot.offset;
                writeIndex(&newroot,newroot.offset);
                writeIndex(&tmpNode,tmpNode.offset);
                writeIndex(index,index->offset);
            }
            else {
                writeIndex(&tmpNode,tmpNode.offset);
                writeIndex(index,index->offset);
                insert_interNode(tmpNode.data[0].k,tmpNode.parent,tmpNode.offset);
            }

        }

        offset_t Findleaf(const Key &key, offset_t offset) {
            interNode rt;
            readIndex(&rt,rootOffSet);
            int low = 0, high = rt.dataSize-1,mid = (low + high)/2;
            while (low < high) {
                if (key < rt.data[mid].k) low = mid;
                else high = mid - 1;
                mid = (low + high) / 2;
            }
            if (rt.isLeaf) {
                /*leafNode leaf;
                readLeaf(&leaf,rt.data[mid].dataPos);
                low = 0; high = leaf.nodeOffset - 1; mid = (low + high) / 2;
                while (low < high) {
                    if (key < leaf.data[mid]) low = mid;
                    else high = mid -1;
                    mid = (low + high) /2;
                }*/
                return rt.data[mid].dataPos;
            }
            else {
                Findleaf(key,rt.data[mid].dataPos);
            }
        }
        /** Insert: Insert certain Key-Value into the database
          *Return a pair, the first of the pair is the iterator point to the new
          *element, the second of the pair is Success if it is successfully inserted
          */
        pair<iterator, OperationResult> insert(const Key& key, const Value& value) {
            sizeLeaf++;
            if (sizeLeaf == 0) {
                leafNode leaf;
                //找到头叶子结点
                readLeaf(&leaf,headLeaf);

                root->data[0].k = key;
                root->data[0].dataPos = 0;
                root->dataSize++;

                pair<iterator,OperationResult > retval = insert_leafNode(key,value,leaf);
                interNode inter;
                offset_t offset = rootOffSet;
                //从叶子结点向内部结点依次插入
                while (offset) {            //直到插到根节点
                    readIndex(&inter,offset);
                    inter.data[0].k = key;
                    writeIndex(&inter,offset);
                    //递归至上一内部结点
                    offset = inter.parent;
                }
                return retval;
            }

            interNode inter;
            leafNode leaf;
            offset_t offset = Findleaf(key,rootOffSet);
            readLeaf(&leaf,offset);
            pair<iterator,OperationResult > retval = insert_leafNode(key,value,leaf);
            return retval;

        }

        // Erase: Erase the Key-Value

        // Return Success if it is successfully erased

        // Return Fail if the key doesn't exist in the database

        OperationResult erase(const Key& key) {

            // TODO erase function

            return Fail;  // If you can't finish erase part, just remaining here.

        }

        // Return a iterator to the beginning

        iterator begin() {}

        const_iterator cbegin() const {}

        // Return a iterator to the end(the next element after the last)

        iterator end() {}

        const_iterator cend() const {}

        // Check whether this BTree is empty

        bool empty() const { return !sizeLeaf;}

        // Return the number of <K,V> pairs

        size_t size() const { return sizeLeaf;}

        // Clear the BTree

        void clear() {}

        /**

         * Returns the number of elements with key

         *   that compares equivalent to the specified argument,

         * The default method of check the equivalence is !(a < b || b > a)

         */

        size_t count(const Key& key) const {}

        /**

         * Finds an element with key equivalent to key.

         * key value of the element to search for.

         * Iterator to an element with key equivalent to key.

         *   If no such element is found, past-the-end (see end()) iterator is

         * returned.

         */

        Value at(Key key) {
            offset_t offset = Findleaf(key,rootOffSet);
            leafNode leaf;
            readLeaf(&leaf,offset);
            for (int i = 0; i < leaf.dataSize; ++i) {
                if (key == leaf.data[i].k) return leaf.data[i].value;
            }
        }

        iterator find(const Key& key) {}

        const_iterator find(const Key& key) const {}

        void BuildTree() {
            End = 0;
            root->offset = End;
            rootOffSet = 0;
            End += sizeof(interNode);
            headLeaf = tailLeaf = End;
            //BTree
            root->parent = 0;
            root->dataSize = 1;
            root->isLeaf = 1;

            leafNode leaf;
            leaf.nodeOffset = End;
            End += sizeof(leafNode);
            root->data[0].dataPos = leaf.nodeOffset;
            leaf.next = leaf.pre = 0;
            leaf.dataSize = 0;
            writeIndex(root,rootOffSet);
            writeLeaf(&leaf,leaf.nodeOffset);

        }

    };

}  // namespace sjtu

