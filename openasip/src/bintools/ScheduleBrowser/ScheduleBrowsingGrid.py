import wx
import re
import  wx.grid as gridlib
from collections import defaultdict
from Utils import dependence_label, constant_factory

class DependenceSummaryFrame(wx.Frame):

    def __init__(self, parent, id, node, incoming_deps, outgoing_deps,
                 pos=wx.DefaultPosition, size=wx.DefaultSize,
                 style=wx.DEFAULT_FRAME_STYLE):
        """Show a summary of dependences. Parameters incoming_deps and outgoing_deps
        are lists of tuples [(node_label, dep_label), ...]
        """

        wx.Frame.__init__(self, parent, id, "Dependences for %s" % node, pos, size, style)

        self.grid_ = gridlib.Grid(self, -1)
        self.grid_.CreateGrid(len(incoming_deps) + len(outgoing_deps), 5)
        row = 0
        for source, dep in incoming_deps:
            self.grid_.SetCellValue(row, 0, source)
            self.grid_.SetCellValue(row, 1, dep)
            self.grid_.SetCellValue(row, 2, node)
            row += 1
        for sink, dep in outgoing_deps:
            self.grid_.SetCellValue(row, 2, node)
            self.grid_.SetCellValue(row, 3, dep)
            self.grid_.SetCellValue(row, 4, sink)
            row += 1
        self.grid_.AutoSizeColumns()
        self.grid_.EnableEditing(False)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.grid_, flag=wx.EXPAND)
        self.SetSizer(sizer)
        self.Fit()


class ScheduleBrowsingGrid(gridlib.Grid):
    
    def __init__(self, parent, schedule, incoming_dependences, outgoing_dependences, labels,
                 dependence_colour, selected_instruction, cursor_instruction):

        gridlib.Grid.__init__(self, parent, -1)

        self.column_to_label_map_ = {}
        self.label_to_column_map_ = {}
        self.schedule_ = schedule
	self.incoming_dependences_ = defaultdict(list)
        self.incoming_dependences_.update(incoming_dependences)
	self.outgoing_dependences_ = defaultdict(list)
        self.outgoing_dependences_.update(outgoing_dependences)
        self.labels_ = labels
        self.highlighted_cells_ = []
        self.node_to_cell_map_ = defaultdict(constant_factory(None))
	self.cell_to_node_map_ = {}
        self.dependence_colour_ = dependence_colour
        self.selected_instruction_ = selected_instruction
        self.cursor_instruction_ = cursor_instruction
        self.include_loop_edges_ = False

        rows = 0
        columns = 0
        
        for cycle in schedule.keys():
            if cycle >= 0:
                rows = max(cycle, rows)

                for slot in schedule[cycle].keys():
                    if not self.label_to_column_map_.has_key(slot):
                        self.column_to_label_map_[columns] = slot
                        self.label_to_column_map_[slot] = columns
                        columns = columns + 1

                    self.node_to_cell_map_[schedule[cycle][slot]] = (cycle, self.label_to_column_map_[slot])
                    self.cell_to_node_map_[(cycle, self.label_to_column_map_[slot])] = schedule[cycle][slot]

	# Schedule cycles are zero-based
	rows = rows + 1

        self.CreateGrid(rows, columns)
        self.Bind(gridlib.EVT_GRID_SELECT_CELL, self.OnSelectCell)
        self.Bind(gridlib.EVT_GRID_CELL_RIGHT_CLICK, self.OnCellRightClick)
        self.Bind(gridlib.EVT_GRID_CELL_LEFT_DCLICK, self.OnCellLeftDClick)

        for col in self.column_to_label_map_.keys():
            self.SetColLabelValue(col, "%s" % self.column_to_label_map_[col])

	for row in range(0, rows):
            self.SetRowLabelValue(row, "%s" % row)
            for col in range(0, columns):
                slot = self.column_to_label_map_[col]
                self.SetCellBackgroundColour(row, col, "white")
                cycle = row
                if schedule.has_key(cycle) and schedule[cycle].has_key(slot):
                    self.SetCellValue(row, col, labels[schedule[cycle][slot]])
                else:
                    self.SetCellValue(row, col, "...")

        self.InstallGridHint(self.GetCellValue)
        self.EnableEditing(False)

    def InstallGridHint(self, rowcolhintcallback):
        prev_rowcol = [None,None]
        def OnMouseMotion(evt):
            # evt.GetRow() and evt.GetCol() would be nice to have here,
            # but as this is a mouse event, not a grid event, they are not
            # available and we need to compute them by hand.
            x, y = self.CalcUnscrolledPosition(evt.GetPosition())
            row = self.YToRow(y)
            col = self.XToCol(x)

            if (row,col) != prev_rowcol and row >= 0 and col >= 0:
                prev_rowcol[:] = [row,col]
                hinttext = rowcolhintcallback(row, col)
                if hinttext is None:
                    hinttext = ''
                self.cursor_instruction_.SetValue(hinttext)
            evt.Skip()

        wx.EVT_MOTION(self.GetGridWindow(), OnMouseMotion)


    def set_loop_edge_inclusion(self, state):
        self.include_loop_edges_ = state

    def unhighlight_cells(self):
        for r,c in self.highlighted_cells_:
            self.SetCellBackgroundColour(r, c, "white")
            self.SetRowLabelValue(r, "%s" % r)

            # Without this, the grid is not always updated on screen.
            self.ForceRefresh()

        self.highlighted_cells_ = []

    def unhighlight_cell(self, row, col):
        self.SetCellBackgroundColour(row, col, "white")
        self.SetRowLabelValue(row, "%s" % row)

        # Without this, the grid is not always updated on screen.
        self.ForceRefresh()

        if (row, col) in self.highlighted_cells_:
            self.highlighted_cells_.remove((row, col))

    def highlight_cell(self, row, column, colour):
        self.highlighted_cells_.append((row, column))
        self.SetCellBackgroundColour(row, column, colour)

        self.SetRowLabelValue(row, "%s ==>" % row)

        # Without this, the grid is not always updated on screen.
        self.ForceRefresh()


    def OnSelectCell(self, evt):
        row = evt.GetRow()
        col = evt.GetCol()
        self.selected_instruction_.SetValue(self.GetCellValue(row, col))
        self.highlight_dependences(row, col)
        evt.Skip()

    def OnCellLeftDClick(self, evt):
        col = evt.GetCol()
        self.AutoSizeColumn(col, False)
        self.ForceRefresh()
        evt.Skip()

    def OnCellRightClick(self, evt):
        row = evt.GetRow()
        col = evt.GetCol()
        if self.cell_to_node_map_.has_key((row, col)):
            incoming = []
            outgoing = []

            this_node = self.cell_to_node_map_[(row, col)]
            for node, kind, latency, loop in self.incoming_dependences_[this_node]:
                if not loop or self.include_loop_edges_:
                    incoming.append((self.labels_[node], dependence_label(kind)))
        
            for node, kind, latency, loop in self.outgoing_dependences_[this_node]:
                if not loop or self.include_loop_edges_:
                    outgoing.append((self.labels_[node], dependence_label(kind)))

            f = DependenceSummaryFrame(self, -1, 
                                       self.labels_[this_node],
                                       incoming,
                                       outgoing)
            f.Show()
            
        evt.Skip()

    def highlight_dependences(self, row, column):
        self.unhighlight_cells()
        if self.cell_to_node_map_.has_key((row, column)):
            incoming_count = 0
            outgoing_count = 0
            
            node = self.cell_to_node_map_[(row, column)]

            for n,t,l,loop in self.incoming_dependences_[node]:
                if not loop or self.include_loop_edges_:
                    incoming_count += 1
                    entry = self.node_to_cell_map_[n]
                    if entry:
                        r, c = entry
                        self.highlight_cell(r, c, self.dependence_colour_[t])

            for n,t,l,loop in self.outgoing_dependences_[node]:
                if not loop or self.include_loop_edges_:
                    outgoing_count += 1
                    entry = self.node_to_cell_map_[n]
                    if entry:
                        r, c = entry
                        self.highlight_cell(r, c, self.dependence_colour_[t])

            self.GetTopLevelParent().SetStatusFields(["%s incoming dependences" % incoming_count,
                                                      "%s outgoing dependences" % outgoing_count])

            self.GetTopLevelParent().Layout()
        else:
            self.GetTopLevelParent().SetStatusFields(["0 incoming dependences",
                                                      "0 outgoing dependences"])

    def highlight_dependence_kind(self, dep_kind, show_source, show_sink):
        """Highlight sources of the specified kind of dependence."""

        self.unhighlight_cells()

        if show_source:
            for deplist in self.incoming_dependences_.values():
                for node, kind, latency, loop in deplist:
                    if kind == dep_kind:
                        entry = self.node_to_cell_map_[node]
                        if entry:
                            r, c = entry
                            self.highlight_cell(r, c, self.dependence_colour_[kind])
        if show_sink:
            for deplist in self.outgoing_dependences_.values():
                for node, kind, latency, loop in deplist:
                    if kind == dep_kind:
                        entry = self.node_to_cell_map_[node]
                        if entry:
                            r, c = entry
                            self.highlight_cell(r, c, self.dependence_colour_[kind])

        self.GetTopLevelParent().SetStatusFields(["", ""])

    def highlight_matching_cells(self, regexp):
        """Highlights cells whose contents matches regexp."""

        self.unhighlight_cells()

        try:
            search_re = re.compile(regexp)
        except:
            self.GetTopLevelParent().SetStatusFields(["Error in search regexp", ""])
            return

        matches = 0
        for row in range(0, self.GetNumberRows()):
            for col in range(0, self.GetNumberCols()):
                if search_re.search(self.GetCellValue(row, col)):
                    self.highlight_cell(row, col, 'yellow')
                    matches = matches + 1

        self.GetTopLevelParent().SetStatusFields(["%d matches" % matches, ""])

    def highlight_resource_constrained_cells(self):
        """Highlight cells whose moves could be scheduled earlier."""

        def earliest_cycle(incoming_deps):
            """Find out the earliest cycle when the move could
            be scheduled based on dependences only."""

            earliest_cycle = 0

            for src_node, dep_kind, latency, loop in incoming_deps:
                entry = self.node_to_cell_map_[src_node]
                if entry:
                    src_row, src_col = entry
                    earliest_cycle = max(earliest_cycle, src_row + latency)

            return earliest_cycle



        highlighted = []
        count = 0

        self.unhighlight_cells()

        for row in range(0, self.GetNumberRows()):
            for col in range(0, self.GetNumberCols()):
                if self.cell_to_node_map_.has_key((row, col)):
                    cell = self.cell_to_node_map_[(row, col)]
                    incoming_deps = self.incoming_dependences_[cell]

                    # Highlight cell if scheduled later.
                    if earliest_cycle(incoming_deps) < row:
                        self.highlight_cell(row, col, 'orange')
                        highlighted.append((row, col))
                        count = count + 1

        for row, col in highlighted:
            cell = self.cell_to_node_map_[(row, col)]
            for dst_node, dep_kind, latency, loop in self.outgoing_dependences_[cell]:

                # If there is a trigger dependence from this move to a triggering
                # move and the trigger is constrained by data dependences, then
                # scheduling this move earlier would not help.
                if (dep_kind == "T"):
                    dst_cell = self.node_to_cell_map_[dst_node]
                    if dst_cell:
                        dst_row, dst_col = dst_cell
                        dst_incoming_deps = self.incoming_dependences_[dst_cell]
                        dst_operation_deps = filter(lambda src, kind, lat: kind == "O",
                                                    dst_incoming_deps)
                        if earliest_cycle(dst_operation_deps) < dst_row:
                            self.highlight_cell(row, col, 'yellow')
                            count = count - 1
                            break
                        
        self.GetTopLevelParent().SetStatusFields(["%d resource constrained moves" % count, ""])
