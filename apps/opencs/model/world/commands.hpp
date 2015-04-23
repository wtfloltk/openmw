#ifndef CSM_WOLRD_COMMANDS_H
#define CSM_WOLRD_COMMANDS_H

#include "record.hpp"

#include <string>
#include <map>
#include <vector>

#include <QVariant>
#include <QUndoCommand>
#include <QModelIndex>

#include "universalid.hpp"
#include "nestedtablewrapper.hpp"

class QModelIndex;
class QAbstractItemModel;

namespace CSVRender
{
    class Cell;
}

namespace CSMWorld
{
    class IdTable;
    class IdTree;
    struct RecordBase;
    struct NestedTableWrapperBase;

    class ModifyCommand : public QUndoCommand
    {
            QAbstractItemModel& mModel;
            QModelIndex mIndex;
            QVariant mNew;
            QVariant mOld;

        public:

            ModifyCommand (QAbstractItemModel& model, const QModelIndex& index, const QVariant& new_,
                QUndoCommand *parent = 0);

            virtual void redo();

            virtual void undo();
    };

    class CreateCommand : public QUndoCommand
    {
            std::map<int, QVariant> mValues;

        protected:

            IdTable& mModel;
            std::string mId;
            UniversalId::Type mType;

        protected:

            /// Apply modifications set via addValue.
            void applyModifications();

        public:

            CreateCommand (IdTable& model, const std::string& id, QUndoCommand *parent = 0);

            void setType (UniversalId::Type type);

            void addValue (int column, const QVariant& value);

            virtual void redo();

            virtual void undo();
    };

    class CloneCommand : public CreateCommand
    {
            std::string mIdOrigin;

        public:

            CloneCommand (IdTable& model, const std::string& idOrigin,
                          const std::string& IdDestination,
                          const UniversalId::Type type,
                          QUndoCommand* parent = 0);

            virtual void redo();

            virtual void undo();
    };

    class RevertCommand : public QUndoCommand
    {
            IdTable& mModel;
            std::string mId;
            RecordBase *mOld;

            // not implemented
            RevertCommand (const RevertCommand&);
            RevertCommand& operator= (const RevertCommand&);

        public:

            RevertCommand (IdTable& model, const std::string& id, QUndoCommand *parent = 0);

            virtual ~RevertCommand();

            virtual void redo();

            virtual void undo();
    };

    class DeleteCommand : public QUndoCommand
    {
            IdTable& mModel;
            std::string mId;
            RecordBase *mOld;

            // not implemented
            DeleteCommand (const DeleteCommand&);
            DeleteCommand& operator= (const DeleteCommand&);

        public:

            DeleteCommand (IdTable& model, const std::string& id, QUndoCommand *parent = 0);

            virtual ~DeleteCommand();

            virtual void redo();

            virtual void undo();
    };

    class ReorderRowsCommand : public QUndoCommand
    {
            IdTable& mModel;
            int mBaseIndex;
            std::vector<int> mNewOrder;

        public:

            ReorderRowsCommand (IdTable& model, int baseIndex, const std::vector<int>& newOrder);

            virtual void redo();

            virtual void undo();
    };

    class NestedTableStoring
    {
        NestedTableWrapperBase* mOld;

    public:
        NestedTableStoring(const IdTree& model, const std::string& id, int parentColumn);

        ~NestedTableStoring();

    protected:

        const NestedTableWrapperBase& getOld() const;
    };

    class DeleteNestedCommand : public QUndoCommand, private NestedTableStoring
    {
            IdTree& mModel;

            std::string mId;

            int mParentColumn;

            int mNestedRow;

        public:

            DeleteNestedCommand (IdTree& model,
                    const std::string& id, int nestedRow, int parentColumn, QUndoCommand* parent = 0);

            virtual void redo();

            virtual void undo();
    };

    class AddNestedCommand : public QUndoCommand, private NestedTableStoring
    {
            IdTree& mModel;

            std::string mId;

            int mNewRow;

            int mParentColumn;

        public:

            AddNestedCommand(IdTree& model,
                    const std::string& id, int nestedRow, int parentColumn, QUndoCommand* parent = 0);

            virtual void redo();

            virtual void undo();
    };

    class ModifyPathgridCommand : public QUndoCommand, private NestedTableStoring
    {
            IdTree& mModel;
            std::string mId;

            int mParentColumn;

            NestedTableWrapperBase* mRecord;
            CSVRender::Cell *mCell;

        public:

            // if newEdges is NULL, only the paths are updated
            ModifyPathgridCommand(IdTree& model,
                    const std::string& id, int parentColumn, CSVRender::Cell *cell,
                    NestedTableWrapperBase* newRecord, QUndoCommand* parent = 0);

            virtual void redo();

            virtual void undo();
    };
}

#endif
