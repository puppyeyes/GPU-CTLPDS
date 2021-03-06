/*
 *  Licensed to the Apache Software Foundation (ASF) under one or more
 *  contributor license agreements.  See the NOTICE file distributed with
 *  this work for additional information regarding copyright ownership.
 *  The ASF licenses this file to You under the Apache License, Version 2.0
 *  (the "License"); you may not use this file except in compliance with
 *  the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */
package org.apache.ivy.ant;

import java.io.File;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.ListIterator;
import java.util.Map;
import java.util.Set;
import java.util.StringTokenizer;

import org.apache.ivy.Ivy;
import org.apache.ivy.core.module.descriptor.DependencyDescriptor;
import org.apache.ivy.core.module.descriptor.ModuleDescriptor;
import org.apache.ivy.core.module.id.ModuleId;
import org.apache.ivy.core.settings.IvySettings;
import org.apache.ivy.core.sort.SortOptions;
import org.apache.ivy.plugins.parser.ModuleDescriptorParserRegistry;
import org.apache.ivy.util.Message;
import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.DirectoryScanner;
import org.apache.tools.ant.types.FileSet;
import org.apache.tools.ant.types.Path;

/**
 * Creates an ant filelist of files (usually build.xml) ordered according to the dependencies
 * declared in ivy files.
 */
public class IvyBuildList extends IvyTask {
    public static final class OnMissingDescriptor {
        public static final String HEAD = "head";
        public static final String TAIL = "tail";
        public static final String SKIP = "skip";
        public static final String FAIL = "fail";
        public static final String WARN = "warn";
        
        private OnMissingDescriptor() {
        }
    }

    public static final String DESCRIPTOR_REQUIRED = "required";
    
    private List buildFileSets = new ArrayList(); // List (FileSet)

    private String reference;

    private boolean haltOnError = true;

    private String onMissingDescriptor = OnMissingDescriptor.HEAD;

    private boolean reverse = false;

    private String ivyFilePath;

    private String root = "*";

    private boolean excludeRoot = false;

    private String leaf = "*";

    private String delimiter = ",";

    private boolean excludeLeaf = false;

    private boolean onlydirectdep = false;

    private String restartFrom = "*";
        
    public void addFileset(FileSet buildFiles) {
        buildFileSets.add(buildFiles);
    }

    public String getReference() {
        return reference;
    }

    public void setReference(String reference) {
        this.reference = reference;
    }

    public String getRoot() {
        return root;
    }

    public void setRoot(String root) {
        this.root = root;
    }

    public boolean isExcludeRoot() {
        return excludeRoot;
    }

    public void setExcludeRoot(boolean root) {
        excludeRoot = root;
    }

    public String getLeaf() {
        return leaf;
    }

    public void setLeaf(String leaf) {
        this.leaf = leaf;
    }

    public boolean isExcludeLeaf() {
        return excludeLeaf;
    }

    public void setExcludeLeaf(boolean excludeLeaf) {
        this.excludeLeaf = excludeLeaf;
    }

    public String getDelimiter() {
        return delimiter;
    }

    public void setDelimiter(String delimiter) {
        this.delimiter = delimiter;
    }

    public boolean getOnlydirectdep() {
        return onlydirectdep;
    }

    public void setOnlydirectdep(boolean onlydirectdep) {
        this.onlydirectdep = onlydirectdep;
    }

    
    public void doExecute() throws BuildException {
        if (reference == null) {
            throw new BuildException("reference should be provided in ivy build list");
        }
        if (buildFileSets.isEmpty()) {
            throw new BuildException(
                    "at least one nested fileset should be provided in ivy build list");
        }

        Ivy ivy = getIvyInstance();
        IvySettings settings = ivy.getSettings();

        ivyFilePath = getProperty(ivyFilePath, settings, "ivy.buildlist.ivyfilepath");

        Path path = new Path(getProject());

        Map buildFiles = new HashMap(); // Map (ModuleDescriptor -> File buildFile)
        List independent = new ArrayList();
        List noDescriptor = new ArrayList();
        Collection mds = new ArrayList();

        Set rootModuleNames = new LinkedHashSet();
        if (!"*".equals(root)) {
            StringTokenizer st = new StringTokenizer(root, delimiter);
            while (st.hasMoreTokens()) {
                rootModuleNames.add(st.nextToken());
            }
        }

        Set leafModuleNames = new LinkedHashSet();
        if (!"*".equals(leaf)) {
            StringTokenizer st = new StringTokenizer(leaf, delimiter);
            while (st.hasMoreTokens()) {
                leafModuleNames.add(st.nextToken());
            }
        }

        Set restartFromModuleNames = new LinkedHashSet();
        if (!"*".equals(restartFrom)) {
            StringTokenizer st = new StringTokenizer(restartFrom, delimiter);
            // Only accept one (first) module
            restartFromModuleNames.add(st.nextToken());
        }
        
        for (ListIterator iter = buildFileSets.listIterator(); iter.hasNext();) {
            FileSet fs = (FileSet) iter.next();
            DirectoryScanner ds = fs.getDirectoryScanner(getProject());
            String[] builds = ds.getIncludedFiles();
            for (int i = 0; i < builds.length; i++) {
                File buildFile = new File(ds.getBasedir(), builds[i]);
                File ivyFile = getIvyFileFor(buildFile);
                if (!ivyFile.exists()) {
                    onMissingDescriptor(buildFile, ivyFile, noDescriptor);
                } else {
                    try {
                        ModuleDescriptor md = ModuleDescriptorParserRegistry.getInstance()
                                .parseDescriptor(
                                    settings, ivyFile.toURI().toURL(), doValidate(settings));
                        buildFiles.put(md, buildFile);
                        mds.add(md);
                        Message.debug("Add " + md.getModuleRevisionId().getModuleId());
                    } catch (Exception ex) {
                        if (haltOnError) {
                            throw new BuildException("impossible to parse ivy file for "
                                    + buildFile + ": ivyfile=" + ivyFile + " exception=" + ex, ex);
                        } else {
                            Message.warn("impossible to parse ivy file for " + buildFile
                                    + ": ivyfile=" + ivyFile + " exception=" + ex.getMessage());
                            Message.info("\t=> adding it at the beginning of the path");
                            independent.add(buildFile);
                        }
                    }
                }
            }
        }

        List leafModuleDescriptors = 
            convertModuleNamesToModuleDescriptors(mds, leafModuleNames, "leaf");
        List rootModuleDescriptors = 
            convertModuleNamesToModuleDescriptors(mds, rootModuleNames, "root");
        List restartFromModuleDescriptors = 
            convertModuleNamesToModuleDescriptors(mds, restartFromModuleNames, "restartFrom");

        if (!rootModuleDescriptors.isEmpty()) {
            Message.info("Filtering modules based on roots " + rootModuleNames);
            mds = filterModulesFromRoot(mds, rootModuleDescriptors);
        }
        if (!leafModuleDescriptors.isEmpty()) {
            Message.info("Filtering modules based on leafs " + leafModuleNames);
            mds = filterModulesFromLeaf(mds, leafModuleDescriptors);
        }

        List sortedModules = ivy.sortModuleDescriptors(mds, SortOptions.DEFAULT);

        if (!OnMissingDescriptor.TAIL.equals(onMissingDescriptor)) {
            for (ListIterator iter = noDescriptor.listIterator(); iter.hasNext();) {
                File buildFile = (File) iter.next();
                addBuildFile(path, buildFile);
            }            
        }
        for (ListIterator iter = independent.listIterator(); iter.hasNext();) {
            File buildFile = (File) iter.next();
            addBuildFile(path, buildFile);
        }
        if (isReverse()) {
            Collections.reverse(sortedModules);
        }
        // Remove modules that are before the restartFrom point
        // Independent modules (without valid ivy file) can not be addressed
        // so they are not removed from build path.
        if (!restartFromModuleDescriptors.isEmpty()) {
            boolean foundRestartFrom = false;
            List keptModules = new ArrayList();
            ModuleDescriptor restartFromModuleDescriptor = 
                (ModuleDescriptor) restartFromModuleDescriptors.get(0);
            for (ListIterator iter = sortedModules.listIterator(); iter.hasNext();) {
                ModuleDescriptor md = (ModuleDescriptor) iter.next();
                if (md.equals(restartFromModuleDescriptor)) {
                    foundRestartFrom = true;
                }
                if (foundRestartFrom) {
                    keptModules.add(md);
                }
            }
            sortedModules = keptModules;
        }
        StringBuffer order = new StringBuffer();
        for (ListIterator iter = sortedModules.listIterator(); iter.hasNext();) {
            ModuleDescriptor md = (ModuleDescriptor) iter.next();
            order.append(md.getModuleRevisionId().getModuleId());
            if (iter.hasNext()) {
                order.append(", ");
            }
            File buildFile = (File) buildFiles.get(md);
            addBuildFile(path, buildFile);
        }
        if (OnMissingDescriptor.TAIL.equals(onMissingDescriptor)) {
            for (ListIterator iter = noDescriptor.listIterator(); iter.hasNext();) {
                File buildFile = (File) iter.next();
                addBuildFile(path, buildFile);
            }            
        }

        getProject().addReference(getReference(), path);
        getProject().setProperty("ivy.sorted.modules", order.toString());
    }

    private void onMissingDescriptor(File buildFile, File ivyFile, List noDescriptor) {
        if (OnMissingDescriptor.SKIP.equals(onMissingDescriptor)) {
            Message.debug("skipping " + buildFile + ": descriptor " + ivyFile
                    + " doesn't exist");
        } else if (OnMissingDescriptor.FAIL.equals(onMissingDescriptor)) {
            throw new BuildException(
                "a module has no module descriptor and onMissingDescriptor=fail. "
                + "Build file: " + buildFile + ". Expected descriptor: " + ivyFile);
        } else {
            if (OnMissingDescriptor.WARN.equals(onMissingDescriptor)) {
                Message.warn(
                    "a module has no module descriptor. "
                    + "Build file: " + buildFile + ". Expected descriptor: " + ivyFile);
            }
            Message.verbose("no descriptor for " + buildFile + ": descriptor=" + ivyFile
                    + ": adding it at the " 
                    + (OnMissingDescriptor.TAIL.equals(onMissingDescriptor) 
                    ? "tail" : "head" + " of the path"));
            Message.verbose(
                "\t(change onMissingDescriptor if you want to take another action");
            noDescriptor.add(buildFile);
        }
    }
    
    private List convertModuleNamesToModuleDescriptors(
            Collection mds, Set moduleNames, String kind) {
        List result = new ArrayList();
        Set foundModuleNames = new HashSet();
        
        for (Iterator it = mds.iterator(); it.hasNext();) {
            ModuleDescriptor md = (ModuleDescriptor) it.next();
            String name = md.getModuleRevisionId().getModuleId().getName();
            if (moduleNames.contains(name)) {
                foundModuleNames.add(name);
                result.add(md);
            }
        }

        if (foundModuleNames.size() < moduleNames.size()) {
            Set missingModules = new HashSet(moduleNames);
            missingModules.removeAll(foundModuleNames);
            
            StringBuffer missingNames = new StringBuffer();
            String sep = "";
            for (Iterator it = missingModules.iterator(); it.hasNext();) {
                missingNames.append(sep);
                missingNames.append(it.next());
                sep = ", ";
            }
            
            throw new BuildException("unable to find " + kind + " module(s) " 
                + missingNames.toString() + " in build fileset");
        }

        return result;
    }

    /**
     * Returns a collection of ModuleDescriptors that are conatined in the input collection of
     * ModuleDescriptors and upon which the root module depends
     * 
     * @param mds
     *            input collection of ModuleDescriptors
     * @param rootmd
     *            root module
     * @return filtered list of modules
     */
    private Collection filterModulesFromRoot(Collection mds, List rootmds) {
        // Make a map of ModuleId objects -> ModuleDescriptors
        Map moduleIdMap = new HashMap();
        for (Iterator iter = mds.iterator(); iter.hasNext();) {
            ModuleDescriptor md = ((ModuleDescriptor) iter.next());
            moduleIdMap.put(md.getModuleRevisionId().getModuleId(), md);
        }

        // recursively process the nodes
        Set toKeep = new LinkedHashSet();

        Iterator it = rootmds.iterator();
        while (it.hasNext()) {
            ModuleDescriptor rootmd = (ModuleDescriptor) it.next();
            processFilterNodeFromRoot(rootmd, toKeep, moduleIdMap);
            // With the excluderoot attribute set to true, take the rootmd out of the toKeep set.
            if (excludeRoot) {
                // Only for logging purposes
                Message.verbose("Excluded module "
                        + rootmd.getModuleRevisionId().getModuleId().getName());
            } else {
                toKeep.add(rootmd);
            }
        }

        // just for logging
        for (Iterator iter = toKeep.iterator(); iter.hasNext();) {
            ModuleDescriptor md = ((ModuleDescriptor) iter.next());
            Message.verbose("Kept module " + md.getModuleRevisionId().getModuleId().getName());
        }

        return toKeep;
    }

    /**
     * Adds the current node to the toKeep collection and then processes the each of the direct
     * dependencies of this node that appear in the moduleIdMap (indicating that the dependency is
     * part of this BuildList)
     * 
     * @param node
     *            the node to be processed
     * @param toKeep
     *            the set of ModuleDescriptors that should be kept
     * @param moduleIdMap
     *            reference mapping of moduleId to ModuleDescriptor that are part of the BuildList
     */
    private void processFilterNodeFromRoot(ModuleDescriptor node, Set toKeep, Map moduleIdMap) {
        //toKeep.add(node);

        DependencyDescriptor[] deps = node.getDependencies();
        for (int i = 0; i < deps.length; i++) {
            ModuleId id = deps[i].getDependencyId();
            ModuleDescriptor md = (ModuleDescriptor) moduleIdMap.get(id);
            // we test if this module id has a module descriptor, and if it isn't already in the 
            // toKeep Set, in which there's probably a circular dependency
            if (md != null && !toKeep.contains(md)) { 
                toKeep.add(md);
                if (!getOnlydirectdep()) {
                    processFilterNodeFromRoot(md, toKeep,
                        moduleIdMap);
                }
            }
        }
    }

    /**
     * Returns a collection of ModuleDescriptors that are conatined in the input collection of
     * ModuleDescriptors which depends on the leaf module
     * 
     * @param mds
     *            input collection of ModuleDescriptors
     * @param leafmd
     *            leaf module
     * @return filtered list of modules
     */
    private Collection filterModulesFromLeaf(Collection mds, List leafmds) {
        // Make a map of ModuleId objects -> ModuleDescriptors
        Map moduleIdMap = new HashMap();
        for (Iterator iter = mds.iterator(); iter.hasNext();) {
            ModuleDescriptor md = ((ModuleDescriptor) iter.next());
            moduleIdMap.put(md.getModuleRevisionId().getModuleId(), md);
        }

        // recursively process the nodes
        Set toKeep = new LinkedHashSet();
        Iterator it = leafmds.iterator();
        while (it.hasNext()) {
            ModuleDescriptor leafmd = (ModuleDescriptor) it.next();
            // With the excludeleaf attribute set to true, take the rootmd out of the toKeep set.
            if (excludeLeaf) {
                Message.verbose("Excluded module "
                        + leafmd.getModuleRevisionId().getModuleId().getName());
            } else {
                toKeep.add(leafmd);
            }
            processFilterNodeFromLeaf(leafmd, toKeep, moduleIdMap);
        }

        // just for logging
        for (Iterator iter = toKeep.iterator(); iter.hasNext();) {
            ModuleDescriptor md = ((ModuleDescriptor) iter.next());
            Message.verbose("Kept module " + md.getModuleRevisionId().getModuleId().getName());
        }

        return toKeep;
    }

    /**
     * Search in the moduleIdMap modules depending on node, add them to the toKeep set and process
     * them recursively.
     * 
     * @param node
     *            the node to be processed
     * @param toKeep
     *            the set of ModuleDescriptors that should be kept
     * @param moduleIdMap
     *            reference mapping of moduleId to ModuleDescriptor that are part of the BuildList
     */
    private void processFilterNodeFromLeaf(ModuleDescriptor node, Set toKeep, Map moduleIdMap) {
        for (Iterator iter = moduleIdMap.values().iterator(); iter.hasNext();) {
            ModuleDescriptor md = (ModuleDescriptor) iter.next();
            DependencyDescriptor[] deps = md.getDependencies();
            for (int i = 0; i < deps.length; i++) {
                ModuleId id = deps[i].getDependencyId();
                if (node.getModuleRevisionId().getModuleId().equals(id) && !toKeep.contains(md)) {
                    toKeep.add(md);
                    if (!getOnlydirectdep()) {
                        processFilterNodeFromLeaf(md, toKeep, moduleIdMap);
                    }
                }
            }
        }
    }

    private void addBuildFile(Path path, File buildFile) {
        FileSet fs = new FileSet();
        fs.setFile(buildFile);
        path.addFileset(fs);
    }

    private File getIvyFileFor(File buildFile) {
        return new File(buildFile.getParentFile(), ivyFilePath);
    }

    public boolean isHaltonerror() {
        return haltOnError;
    }

    public void setHaltonerror(boolean haltOnError) {
        this.haltOnError = haltOnError;
    }

    public String getIvyfilepath() {
        return ivyFilePath;
    }

    public void setIvyfilepath(String ivyFilePath) {
        this.ivyFilePath = ivyFilePath;
    }

    public String getOnMissingDescriptor() {
        return onMissingDescriptor;
    }
    
    public void setOnMissingDescriptor(String onMissingDescriptor) {
        this.onMissingDescriptor = onMissingDescriptor;
    }
    
    /**
     * @deprecated use {@link #getOnMissingDescriptor()} instead. 
     */
    public boolean isSkipbuildwithoutivy() {
        return onMissingDescriptor == OnMissingDescriptor.SKIP;
    }

    /**
     * @deprecated use {@link #setOnMissingDescriptor(String)} instead. 
     */
    public void setSkipbuildwithoutivy(boolean skipBuildFilesWithoutIvy) {
        Message.deprecated("skipbuildwithoutivy is deprecated, use onMissingDescriptor instead.");
        this.onMissingDescriptor = skipBuildFilesWithoutIvy
            ? OnMissingDescriptor.SKIP
                    : OnMissingDescriptor.FAIL;
    }

    public boolean isReverse() {
        return reverse;
    }

    public void setReverse(boolean reverse) {
        this.reverse = reverse;
    }

    public String getRestartFrom() {
        return restartFrom;
    }

    public void setRestartFrom(String restartFrom) {
        this.restartFrom = restartFrom;
    }

    
}
